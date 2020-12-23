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

#include <src/engine/loader/DynamicLoaderCollection.h>
#include <src/graphics/DynamicMeshHandle.h>
#include <src/world/terrain/TerrainMeshUpdater.h>

class World;

struct VistaChunkRequest
{
    u32 m_LOD = 0;
    glm::ivec3 m_Index;
    bool m_IsFringe = false;
};

struct VistaChunkIdentifier
{
    u32 m_LOD = 0;
    glm::ivec3 m_Coords;
    u32 m_Generation = 0;

    bool operator==(const VistaChunkIdentifier& _other)
    {
        return m_LOD == _other.m_LOD && m_Coords == _other.m_Coords;
    };
};

// The Vista is the visible world beyond the range of the active zones.
class VistaHandler
{
public:
    explicit VistaHandler(const World* _world);
    void Update(TimeMS _dt);
    void OnLocalPlayerWorldZoneChanged(glm::ivec3 _zone);

    glm::mat4 GetTerrainModelTransform() const;

public:
    mutable DynamicMeshHandle m_DynamicMesh;

private:
    glm::vec3 GetGlobalSamplingOffset(glm::ivec3 _coords, u32 _lod) const;
    glm::vec3 GetGlobalPositionOffset(glm::ivec3 _coords, u32 _lod) const;
    u32 GetZonesOffsetFromCenter(u32 _lod) const;
    TerrainProperties GetPropertiesForLOD(u32 _lod) const;

    void GenerateChunkRequestsForLOD(u32 _lod);
private:
    const World* m_World = nullptr;

    RawMesh m_CombinedRawMesh;
    bool m_WaitingToUpdateDynamicMesh = false;
    bool m_WaitingToUpdateTransform = false;

    glm::ivec3 m_VistaOrigin;

    glm::mat4 m_TerrainModelTransform;

    static constexpr u32 CONCURRENT_TERRAIN_UPDATERS_COUNT = 3;
    using TerrainMeshUpdaters = DynamicLoaderCollection<TerrainMeshUpdater, VistaChunkIdentifier, CONCURRENT_TERRAIN_UPDATERS_COUNT, TerrainMeshUpdateParams>;
    TerrainMeshUpdaters m_TerrainMeshUpdaters;

    std::vector<VistaChunkRequest> m_PendingRequests;

    // We can't stop TerrainMeshUpdates that are in flight, but we need to discard them if they're out of date when they
    // arrive. This is a hacky fix for that, for now.
    u32 m_Generation = 0;

    static std::array<u32, 3> VERTS_PER_EDGE_FOR_LOD;
    static std::array<u32, 3> ZONES_PER_EDGE_FOR_LOD;
    static constexpr u32 LODS_IN_USE_COUNT = 3;

    static_assert(VistaHandler::VERTS_PER_EDGE_FOR_LOD.size() == VistaHandler::ZONES_PER_EDGE_FOR_LOD.size());
    static_assert(VistaHandler::VERTS_PER_EDGE_FOR_LOD.size() >= LODS_IN_USE_COUNT);
};
