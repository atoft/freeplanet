//
// Created by alastair on 21/03/2020.
//

#pragma once

#include <src/world/collision/CollisionHelpers.h>

class TerrainComponent;

class CollideOBBTerrain
{
public:
    static std::optional<CollisionResult> Collide(const glm::mat4& _transform1, const AABB& _bounds1, const TerrainComponent& _terrain, const glm::vec3& _terrainOffset);
};
