//
// Created by alastair on 08/09/19.
//


#pragma once

#include <glm/glm.hpp>

#include <src/graphics/RawMesh.h>
#include <src/world/terrain/NormalGenerationMethod.h>
#include <src/world/terrain/TerrainEdits.h>
#include <src/world/GeometryTypes.h>
#include <src/world/planet/TerrainGeneration.h>
#include <src/world/terrain/TerrainHelpers.h>

struct Planet;

struct TerrainRegion
{
    glm::ivec3 m_Min;
    glm::ivec3 m_Max;
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
};

class TerrainMeshUpdater
{
public:
    explicit TerrainMeshUpdater(TerrainMeshUpdateParams _params);

    RawMesh GetRawMesh() const { return m_Mesh; };
    std::vector<TerrainChunk> GetChunks() const { return m_Chunks; };

private:
    void UpdateChunks(const TerrainMeshUpdateParams& _params, std::vector<TerrainChunk>& _existingChunks) const;
    void ConvertToRawMesh(const TerrainMeshUpdateParams& _params, const std::vector<TerrainChunk>& _existingChunks, RawMesh& _outRawMesh) const;

private:
    RawMesh m_Mesh;
    std::vector<TerrainChunk> m_Chunks;
};
