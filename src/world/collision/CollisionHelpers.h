//
// Created by alastair on 27/12/17.
//

#pragma once

#include <optional>
#include <glm/vec3.hpp>

#include <src/tools/globals.h>
#include <src/world/WorldObject.h>

struct Triangle;

struct CollisionResult
{
    glm::vec3 m_Normal = glm::vec3();
    float m_Distance = 0.f;

    std::string m_DebugInfo;
};

namespace CollisionHelpers
{
    struct OBBProperties
    {
        glm::vec3 m_Origin;
        glm::vec3 m_X;
        glm::vec3 m_Y;
        glm::vec3 m_Z;
        std::array<glm::vec3, 8> m_Coordinates;
    };

    bool IsIntersecting(glm::vec3 _position1, AABB _aabb1, glm::vec3 _position2, AABB _aabb2);

    inline bool CollisionInAxis(int _index, glm::vec3 _position1, glm::vec3 _halfDimensions1,
                                glm::vec3 _position2, glm::vec3 _halfDimensions2);

    glm::vec3 Rotate(glm::vec3 position, glm::vec3 eulerAngles);

    // Compute various properties needed in OBB collision algorithms
    OBBProperties GetOBBProperties(const glm::mat4& _transform, const AABB& _bounds, const glm::vec3& _positionOffset);

    AABB GetAABBForOBB(const glm::mat4& _rotation, const AABB& _bounds);
}
