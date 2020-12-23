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

#include <glm/glm.hpp>

#include <src/graphics/RawMesh.h>
#include <src/world/terrain/NormalGenerationMethod.h>
#include <src/world/terrain/TerrainEdits.h>
#include <src/world/GeometryTypes.h>
#include <src/world/planet/TerrainGeneration.h>
#include <src/world/terrain/TerrainHelpers.h>
#include <src/world/events/WorldEvent.h>

struct Planet;

struct TerrainRegion
{
    glm::ivec3 m_Min;
    glm::ivec3 m_Max;

    TerrainRegion Union(const TerrainRegion& _other) const;
};

struct TerrainMeshUpdateParams
{
    glm::ivec3 m_ZoneCoordinates;
    const Planet* m_Planet = nullptr;
    TerrainEdits m_TerrainEdits;
    std::vector<TerrainChunk> m_ExistingChunks;
    TerrainProperties m_Properties;
    TerrainRegion m_DirtyRegion;
    NormalGenerationMethod m_NormalGenerationMethod = NormalGenerationMethod::FromVolume;
    TerrainLevelOfDetail m_LevelOfDetail = TerrainLevelOfDetail::ActiveZone;
    bool m_IsFringe = false;
    std::vector<WorldEvent> m_AssociatedEvents;
};

class TerrainMeshUpdater
{
public:
    explicit TerrainMeshUpdater(TerrainMeshUpdateParams _params);

    RawMesh GetRawMesh() const { return m_Mesh; };
    std::vector<TerrainChunk> GetChunks() const { return m_Chunks; };
    std::vector<WorldEvent> GetAssociatedEvents() const { return m_AssociatedEvents; };

private:
    void UpdateChunks(const TerrainMeshUpdateParams& _params, std::vector<TerrainChunk>& _existingChunks) const;
    void ConvertToRawMesh(const TerrainMeshUpdateParams& _params, const std::vector<TerrainChunk>& _existingChunks, RawMesh& _outRawMesh) const;

private:
    RawMesh m_Mesh;
    std::vector<TerrainChunk> m_Chunks;
    std::vector<WorldEvent> m_AssociatedEvents;
};
