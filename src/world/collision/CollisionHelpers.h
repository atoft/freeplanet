//
// Created by alastair on 27/12/17.
//

#pragma once

#include <optional>
#include <glm/vec3.hpp>

#include <src/tools/globals.h>
#include <src/world/WorldObject.h>

struct Triangle;

namespace CollisionHelpers
{
    struct OBBProperties
    {
        glm::vec3 m_X;
        glm::vec3 m_Y;
        glm::vec3 m_Z;
        std::array<glm::vec3, 8> m_Coordinates;
    };

    bool IsIntersecting(glm::vec3 _position1, AABB _aabb1, glm::vec3 _position2, AABB _aabb2);

    inline bool CollisionInAxis(int _index, glm::vec3 _position1, glm::vec3 _halfDimensions1,
                                glm::vec3 _position2, glm::vec3 _halfDimensions2);

    glm::vec3 Rotate(glm::vec3 position, glm::vec3 eulerAngles);

    std::optional<glm::vec3> TryNormalizedCross(glm::vec3 _a, glm::vec3 _b);

    // Project OBBs onto an axis and return their separation distance
    float GetProjectedIntersectionDistance(const std::array<glm::vec3, 8>& _obb1,
                                           const std::array<glm::vec3, 8>& _obb2,
                                           const glm::vec3& _projectionAxis,
                                           bool& _outIsPos);

    float GetProjectedIntersectionDistance(const std::array<glm::vec3, 8>& _obb, const Triangle& _triangle,
                                           const glm::vec3& _projectionAxis, bool& _outIsPosDir);


    bool IsOBBCollisionInAxis(
            const std::array<glm::vec3, 8>& obb1,
            const std::array<glm::vec3, 8>& obb2,
            const std::optional<glm::vec3>& _projectionAxis,
            float& _minIntersect,
            glm::vec3& _axisOfMinIntersect);

    bool IsTriangleCollisionInAxis(const std::array<glm::vec3, 8>& _obb, const Triangle& _triangle,
                                   const std::optional<glm::vec3>& _projectionAxis, f32& _outSeparation, glm::vec3& _inOutAxisOfMinIntersect);


    // Compute various properties needed in OBB collision algorithms
    OBBProperties GetOBBProperties(const glm::mat4& _transform, const AABB& _bounds, const glm::vec3& _positionOffset);

}
