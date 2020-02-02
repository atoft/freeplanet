//
// Created by alastair on 11/02/19.
//

#pragma once

#include <glm/vec3.hpp>
#include <optional>
#include <vector>

#include <src/world/GeometryTypes.h>

struct CollisionResult
{
    glm::vec3 m_Normal = glm::vec3();
    float m_Distance = 0.f;
};

class CollisionAlgorithms
{
public:
    static std::optional<CollisionResult> CollideOBB_OBB(
            const glm::mat4& _transform1,
            const AABB& _bounds1,
            const glm::mat4& _transform2,
            const AABB& _bounds2);

    static std::optional<CollisionResult> CollideOBB_Terrain(
            const glm::mat4& _transform1,
            const AABB& _bounds1,
            const std::vector<TerrainChunk>& _terrain,
            const glm::vec3& _terrainOffset);
};

