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
#include <vector>
#include <array>

#include <src/graphics/DynamicMeshHandle.h>
#include <src/tools/globals.h>
#include <src/world/GeometryTypes.h>
#include <src/world/terrain/TerrainEdits.h>
#include <src/world/terrain/TerrainMeshUpdater.h>
#include <src/world/events/WorldEvent.h>

class TerrainComponent
{
    friend class CollisionHandler;
    friend class TerrainHandler;

public:
    TerrainComponent(u32 _chunksPerEdge, f32 _chunkSize, glm::ivec3 _zoneCoords);
    TerrainComponent();

    u32 GetDimensions() const { return m_Properties.m_ChunksPerEdge; };

    // For now we keep this accessible to be used by collision.
    // It wouldn't be too difficult to extract the triangles from the RawMesh and no longer use Chunks in physics.
    // Even then it may be worth caching them internally so that we don't have to regenerate all the chunks
    // in the TerrainMeshUpdater every time there's a change to only part of the Terrain.
    const std::vector<TerrainChunk>& GetChunks() const { return m_TerrainChunks; };

public:
    mutable DynamicMeshHandle m_DynamicMesh;

    std::vector<TerrainChunk> m_TerrainChunks;

    TerrainProperties m_Properties;
    TerrainEdits m_TerrainEdits;

    glm::ivec3 m_ZoneCoordinates;

    std::optional<TerrainRegion> m_DirtyRegion;
    std::vector<WorldEvent> m_AssociatedEvents;
};
