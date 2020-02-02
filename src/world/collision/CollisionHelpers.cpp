//
// Created by alastair on 27/12/17.
//
#define GLM_ENABLE_EXPERIMENTAL

#include "CollisionHelpers.h"

#include <glm/vec3.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <optional>

#include <src/tools/MathsHelpers.h>
#include <src/world/WorldObject.h>
#include <src/world/GeometryTypes.h>

bool CollisionHelpers::IsIntersecting(glm::vec3 _position1, AABB _aabb1, glm::vec3 _position2,
                                      AABB _aabb2)
{
    _position1 += _aabb1.m_PositionOffset;
    _position2 += _aabb2.m_PositionOffset;

    glm::vec3 halfDimensions1 = _aabb1.m_Dimensions;
    glm::vec3 halfDimensions2 = _aabb2.m_Dimensions;

    int axis = 0;
    for(; axis < 3; axis++)
    {
        if(!CollisionInAxis(axis, _position1, halfDimensions1, _position2, halfDimensions2))
            return false;
    }
    return true;
}

bool CollisionHelpers::CollisionInAxis(int _index, glm::vec3 _position1, glm::vec3 _halfDimensions1, glm::vec3 _position2,
                                       glm::vec3 _halfDimensions2)
{
    return ( glm::abs(_position1[_index] - _position2[_index]) < (_halfDimensions1[_index] + _halfDimensions2[_index])  );
}

glm::vec3 CollisionHelpers::Rotate(glm::vec3 position, glm::vec3 eulerAngles)
{
    return glm::rotateX(glm::rotateY(glm::rotateZ(position, eulerAngles.z), eulerAngles.y), eulerAngles.x);
}

std::optional<glm::vec3> CollisionHelpers::TryNormalizedCross(glm::vec3 _a, glm::vec3 _b)
{
    assert(_a != glm::vec3());
    assert(_b != glm::vec3());

    if (glm::epsilonEqual(1.f, glm::abs(glm::dot(_a, _b)), glm::epsilon<f32>()))
    {
        return std::nullopt;
    }
    else
    {
        glm::vec3 result = glm::cross(_a, _b);

        assert(!glm::isnan(result.x));
        assert(!glm::isnan(result.y));
        assert(!glm::isnan(result.z));

        return glm::normalize(result);
    }
}

float CollisionHelpers::GetProjectedIntersectionDistance(const std::array<glm::vec3, 8>& _obb1,
                                                         const std::array<glm::vec3, 8>& _obb2,
                                                         const glm::vec3& _projectionAxis, bool& _outIsPos)
{
    float min1 = std::numeric_limits<float>::max();
    float max1 = -std::numeric_limits<float>::max();

    float min2 = std::numeric_limits<float>::max();
    float max2 = -std::numeric_limits<float>::max();

    assert(!glm::isnan(_projectionAxis.x));
    assert(!glm::isnan(_projectionAxis.y));
    assert(!glm::isnan(_projectionAxis.z));

    for(int i = 0; i < 8; ++i)
    {
        float distOnAxis1 = glm::dot(_obb1[i], _projectionAxis);

        // If further along the axis in + or - dir, set as our max or min
        max1 = glm::max(max1, distOnAxis1);
        min1 = glm::min(min1, distOnAxis1);

        float distOnAxis2 = glm::dot(_obb2[i], _projectionAxis);

        // If further along the axis in + or - dir, set as our max or min
        max2 = glm::max(max2, distOnAxis2);
        min2 = glm::min(min2, distOnAxis2);
    }

    float span = glm::max(max1, max2) - glm::min(min1, min2);
    float totalLength = max1 - min1 + max2 - min2;

    _outIsPos = (max2 > max1);

    return span - totalLength;	// Negative if overlapping
}

float CollisionHelpers::GetProjectedIntersectionDistance(const std::array<glm::vec3, 8>& _obb,
                                                         const Triangle& _triangle,
                                                         const glm::vec3& _projectionAxis,
                                                         bool& _outIsPosDir)
{
    float min1 = std::numeric_limits<float>::max();
    float max1 = -std::numeric_limits<float>::max();

    float min2 = std::numeric_limits<float>::max();
    float max2 = -std::numeric_limits<float>::max();

    assert(!glm::isnan(_projectionAxis.x));
    assert(!glm::isnan(_projectionAxis.y));
    assert(!glm::isnan(_projectionAxis.z));

    for (u32 i = 0; i < 8; ++i)
    {
        const f32 distOnAxis1 = glm::dot(_obb[i], _projectionAxis);

        // If further along the axis in + or - dir, set as our max or min
        max1 = glm::max(max1, distOnAxis1);
        min1 = glm::min(min1, distOnAxis1);
    }
    for (u32 i = 0; i < 3; ++i)
    {
        const f32 distOnAxis2 = glm::dot(_triangle.m_Vertices[i], _projectionAxis);

        // If further along the axis in + or - dir, set as our max or min
        max2 = glm::max(max2, distOnAxis2);
        min2 = glm::min(min2, distOnAxis2);
    }

    _outIsPosDir = (max2 > max1);

    const f32 span = glm::max(max1, max2) - glm::min(min1, min2);
    const f32 totalLength = (max1 - min1) + (max2 - min2);

    return span - totalLength;	// Negative if overlapping
}

bool CollisionHelpers::IsOBBCollisionInAxis(const std::array<glm::vec3, 8>& obb1,
                                            const std::array<glm::vec3, 8>& obb2,
                                            const std::optional<glm::vec3>& _projectionAxis,
                                            float& _minIntersect, glm::vec3& _axisOfMinIntersect)
{
    if(!_projectionAxis.has_value())
    {
        // We have OBBs that have aligned axes
        return true;
    }

    bool isPosDir = false;
    const f32 separation = GetProjectedIntersectionDistance(obb1, obb2, *_projectionAxis, isPosDir);
    if (separation > 0)
    {
        return false;
    }
    // This is a smaller intersection dist than the current one, so update the intersect axis
    if(separation > _minIntersect)
    {

        _minIntersect = separation;
        _axisOfMinIntersect = isPosDir ? (*_projectionAxis) : -(*_projectionAxis);
    }
    return true;
}

bool CollisionHelpers::IsTriangleCollisionInAxis(const std::array<glm::vec3, 8>& _obb,
                                                 const Triangle& _triangle,
                                                 const std::optional<glm::vec3>& _projectionAxis,
                                                 f32& _inOutMinIntersect,
                                                 glm::vec3& _inOutAxisOfMinIntersect)
{
    if (!_projectionAxis.has_value())
    {
        // We have OBBs that have aligned axes
        return true;
    }

    bool isPosDir = false;
    const f32 separation = GetProjectedIntersectionDistance(_obb, _triangle, *_projectionAxis, isPosDir);

    if (separation <= 0.f && separation > _inOutMinIntersect)
    {
        _inOutMinIntersect = separation;
        _inOutAxisOfMinIntersect = *_projectionAxis;// : -(*_projectionAxis);
    }

    return separation <= 0.f;
}

CollisionHelpers::OBBProperties CollisionHelpers::GetOBBProperties(const glm::mat4& _transform, const AABB& _bounds, const glm::vec3& _positionOffset)
{
    const glm::vec3 obbCentre = MathsHelpers::GetPosition(_transform) + _bounds.m_PositionOffset - _positionOffset;

    CollisionHelpers::OBBProperties properties;

    properties.m_X = MathsHelpers::GetRightVector(_transform);
    properties.m_Y = MathsHelpers::GetUpVector(_transform);
    properties.m_Z = MathsHelpers::GetForwardVector(_transform);

    const glm::vec3 obbExtents = _bounds.m_Dimensions;

    properties.m_Coordinates[0] = obbCentre + properties.m_X * obbExtents.x + properties.m_Y * obbExtents.y + properties.m_Z * obbExtents.z;
    properties.m_Coordinates[1] = obbCentre - properties.m_X * obbExtents.x + properties.m_Y * obbExtents.y + properties.m_Z * obbExtents.z;

    properties.m_Coordinates[2] = obbCentre + properties.m_X * obbExtents.x - properties.m_Y * obbExtents.y + properties.m_Z * obbExtents.z;
    properties.m_Coordinates[3] = obbCentre - properties.m_X * obbExtents.x - properties.m_Y * obbExtents.y + properties.m_Z * obbExtents.z;

    properties.m_Coordinates[4] = obbCentre + properties.m_X * obbExtents.x + properties.m_Y * obbExtents.y - properties.m_Z * obbExtents.z;
    properties.m_Coordinates[5] = obbCentre - properties.m_X * obbExtents.x + properties.m_Y * obbExtents.y - properties.m_Z * obbExtents.z;

    properties.m_Coordinates[6] = obbCentre + properties.m_X * obbExtents.x - properties.m_Y * obbExtents.y - properties.m_Z * obbExtents.z;
    properties.m_Coordinates[7] = obbCentre - properties.m_X * obbExtents.x - properties.m_Y * obbExtents.y - properties.m_Z * obbExtents.z;

    return properties;
}
