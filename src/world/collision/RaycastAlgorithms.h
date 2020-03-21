//
// Created by alastair on 06/10/19.
//

#pragma once

#include <optional>
#include <vector>

#include <src/world/WorldObjectID.h>

struct TerrainChunk;

struct RaycastResult
{
    f32 m_Distance = -1.f;
    WorldObjectID m_HitObjectID = WORLDOBJECTID_INVALID;
};

class RaycastAlgorithms
{
public:
    static std::optional<RaycastResult> RaycastTerrain(
            const glm::vec3& _localOrigin,
            const glm::vec3& _direction,
            const std::vector<TerrainChunk>& _terrain);
};
