/*
 * Copyright 2017-2020 Alastair Toft
 *
 * This file is part of freeplanet.
 *
 * freeplanet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeplanet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeplanet. If not, see <http://www.gnu.org/licenses/>.
 */

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
