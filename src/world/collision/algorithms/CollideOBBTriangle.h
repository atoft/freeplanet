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

class CollideOBBTriangle
{
public:
    static std::optional<CollisionResult> Collide(const glm::mat4& _transform, const AABB& _bounds, const Triangle& _triangle, const glm::vec3& _triangleOffset);
    static std::optional<CollisionResult> Collide(const CollisionHelpers::OBBProperties& _obb, const Triangle& _triangle);

private:
    static bool TestAxis(
            const glm::vec3& _axis,
            const CollisionHelpers::OBBProperties& _obb,
            const Triangle& _triangle,
            glm::vec3& _inOutAxis,
            f32& _inOutPenetration,
            std::string& _outDebugInfo);

    static bool TestCrossProductAxis(
            const glm::vec3& _axisA,
            const glm::vec3& _axisB,
            const CollisionHelpers::OBBProperties& _obb,
            const Triangle& _triangle,
            glm::vec3& _inOutAxis,
            f32& _inOutPenetration,
            std::string& _outDebugInfo);

    static bool TestTriangleNormalAxis(
            const CollisionHelpers::OBBProperties& _obb,
            const Triangle& _triangle,
            glm::vec3& _inOutAxis,
            f32& _inOutPenetration, std::string& _outDebugInfo);
};
