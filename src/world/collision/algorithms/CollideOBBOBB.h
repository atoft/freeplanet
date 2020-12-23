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

#include <src/world/collision/CollisionHelpers.h>

class CollideOBBOBB
{
public:
    static std::optional<CollisionResult> Collide(const glm::mat4& _transform1, const AABB& _bounds1, const glm::mat4& _transform2, const AABB& _bounds2);

private:
    static bool TestAxis(
            const std::array<glm::vec3, 8>& _obb1,
            const std::array<glm::vec3, 8>& _obb2,
            const glm::vec3& _projectionAxis,
            f32& _inOutPenetration,
            glm::vec3& _inOutAxis);

    static bool TestCrossProductAxis(
            const std::array<glm::vec3, 8>& _obb1,
            const std::array<glm::vec3, 8>& _obb2,
            const glm::vec3& _axisA,
            const glm::vec3& _axisB,
            f32& _inOutPenetration,
            glm::vec3& _inOutAxis);
};


