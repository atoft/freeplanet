//
// Created by alastair on 21/03/2020.
//

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
