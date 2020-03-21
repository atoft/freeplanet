//
// Created by alastair on 21/03/2020.
//

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


