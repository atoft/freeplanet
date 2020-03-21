//
// Created by alastair on 21/03/2020.
//

#include "CollideOBBTriangle.h"

std::optional<CollisionResult> CollideOBBTriangle::Collide(const glm::mat4& _transform, const AABB& _bounds, const Triangle& _triangle, const glm::vec3& _triangleOffset)
{
    const CollisionHelpers::OBBProperties obb = CollisionHelpers::GetOBBProperties(_transform, _bounds, _triangleOffset);

    return Collide(obb, _triangle);
}

std::optional<CollisionResult> CollideOBBTriangle::Collide(const CollisionHelpers::OBBProperties& _obb, const Triangle& _triangle)
{
    if (_triangle.m_Vertices[1] ==  _triangle.m_Vertices[0] || _triangle.m_Vertices[2] ==  _triangle.m_Vertices[1] || _triangle.m_Vertices[2] ==  _triangle.m_Vertices[0])
    {
        return std::nullopt;
    }

    const glm::vec3 triEdge0 = glm::normalize(_triangle.m_Vertices[1] - _triangle.m_Vertices[0]);
    const glm::vec3 triEdge1 = glm::normalize(_triangle.m_Vertices[2] - _triangle.m_Vertices[1]);
    const glm::vec3 triEdge2 = glm::normalize(_triangle.m_Vertices[0] - _triangle.m_Vertices[2]);

    glm::vec3 chosenAxis = glm::vec3();
    f32 penetration = std::numeric_limits<f32>::max();
    std::string debug;

    if (
               !TestCrossProductAxis(_obb.m_X, triEdge0, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestCrossProductAxis(_obb.m_X, triEdge1, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestCrossProductAxis(_obb.m_X, triEdge2, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestCrossProductAxis(_obb.m_Y, triEdge0, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestCrossProductAxis(_obb.m_Y, triEdge1, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestCrossProductAxis(_obb.m_Y, triEdge2, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestCrossProductAxis(_obb.m_Z, triEdge0, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestCrossProductAxis(_obb.m_Z, triEdge1, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestCrossProductAxis(_obb.m_Z, triEdge2, _obb, _triangle, chosenAxis, penetration, debug))
    {
        return std::nullopt;
    }

    if (
               !TestAxis(_obb.m_X, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestAxis(_obb.m_Y, _obb, _triangle, chosenAxis, penetration, debug)
            || !TestAxis(_obb.m_Z, _obb, _triangle, chosenAxis, penetration, debug))
    {
        return std::nullopt;
    }

    if (!TestTriangleNormalAxis(_obb, _triangle, chosenAxis, penetration, debug))
    {
        return std::nullopt;
    }

    CollisionResult result;
    result.m_Normal = chosenAxis;
    result.m_Distance = -penetration;
    result.m_DebugInfo = debug;
    return result;
}

bool CollideOBBTriangle::TestAxis(
        const glm::vec3& _axis,
        const CollisionHelpers::OBBProperties& _obb,
        const Triangle& _triangle,
        glm::vec3& _inOutAxis,
        f32& _inOutPenetration, std::string& _outDebugInfo)
{
    f32 obbMax = -std::numeric_limits<f32>::max();
    f32 obbMin = std::numeric_limits<f32>::max();

    for (const glm::vec3& obbPoint : _obb.m_Coordinates)
    {
        const f32 distanceOnAxis = glm::dot(obbPoint, _axis);

        obbMax = glm::max(obbMax, distanceOnAxis);
        obbMin = glm::min(obbMin, distanceOnAxis);
    }

    f32 triMax = -std::numeric_limits<f32>::max();
    f32 triMin = std::numeric_limits<f32>::max();

    for (const glm::vec3& triPoint : _triangle.m_Vertices)
    {
        const f32 distanceOnAxis = glm::dot(triPoint, _axis);

        triMax = glm::max(triMax, distanceOnAxis);
        triMin = glm::min(triMin, distanceOnAxis);
    }

    const f32 obbRange = obbMax - obbMin;
    const f32 triRange = triMax - triMin;

    if (glm::epsilonEqual(triMax, triMin, glm::epsilon<f32>()))
    {
        // The triangle normal is closely aligned to _axis.
        // Skip this test as the penetration in the normal direction will
        // be handled in TestTriangleNormalAxis.
        return true;
    }

    const f32 range = obbRange + triRange;
    const f32 span = glm::max(obbMax, triMax) - glm::min(obbMin, triMin);

    if (range >= span)
    {
        const f32 penetration = range - span;

        const f32 obbCenter = obbMin + (obbMax - obbMin) / 2.f;
        const f32 triCenter = triMin + (triMax - triMin) / 2.f;

        assert(penetration >= 0.f);
        if (penetration < _inOutPenetration)
        {
            // Always want a normal pointed towards the intersecting triangle.
            const glm::vec3 direction = obbCenter < triCenter ? _axis : -_axis;

            _inOutPenetration = penetration;
            _inOutAxis = direction;
            _outDebugInfo = "Axis";
        }

        return true;
    }

    return false;
}

bool CollideOBBTriangle::TestCrossProductAxis(
        const glm::vec3& _axisA,
        const glm::vec3& _axisB,
        const CollisionHelpers::OBBProperties& _obb,
        const Triangle& _triangle,
        glm::vec3& _inOutAxis,
        f32& _inOutPenetration, std::string& _outDebugInfo)
{
    if (glm::abs(glm::dot(_axisA, _axisB)) > 0.99f)
    {
        // The axes are close to parallel, so skip this test.
        return true;
    }

    const glm::vec3 testAxis = glm::normalize(glm::cross(_axisA, _axisB));

    const f32 oldPenetration = _inOutPenetration;

    const bool result = TestAxis(testAxis, _obb, _triangle, _inOutAxis, _inOutPenetration, _outDebugInfo);

    if (oldPenetration != _inOutPenetration)
    {
        _outDebugInfo = "Cross Product Axis";
    }

    return result;
}

bool CollideOBBTriangle::TestTriangleNormalAxis(
        const CollisionHelpers::OBBProperties& _obb,
        const Triangle& _triangle,
        glm::vec3& _inOutAxis,
        f32& _inOutPenetration, std::string& _outDebugInfo)
{
    const glm::vec3 axis = glm::normalize(
            glm::cross(
                    glm::normalize(_triangle.m_Vertices[1] - _triangle.m_Vertices[0]),
                    glm::normalize(_triangle.m_Vertices[2] - _triangle.m_Vertices[1])));

    f32 obbMax = -std::numeric_limits<f32>::max();
    f32 obbMin = std::numeric_limits<f32>::max();

    for (const glm::vec3& obbPoint : _obb.m_Coordinates)
    {
        const f32 distanceOnAxis = glm::dot(obbPoint, axis);

        obbMax = glm::max(obbMax, distanceOnAxis);
        obbMin = glm::min(obbMin, distanceOnAxis);
    }

    const f32 triDistance = glm::dot(_triangle.m_Vertices[0], axis);

    if (obbMax > triDistance && obbMin < triDistance)
    {
        const f32 penetration = triDistance - obbMin;

        assert(penetration >= 0.f);
        if (penetration < _inOutPenetration)
        {
            _inOutPenetration = penetration;

            // Always want a normal pointed towards the intersecting triangle.
            _inOutAxis = -axis;

            _outDebugInfo = "Triangle Normal";
        }

        return true;
    }

    return false;
}
