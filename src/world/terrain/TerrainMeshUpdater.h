//
// Created by alastair on 08/09/19.
//


#pragma once

#include <glm/glm.hpp>

#include <src/graphics/RawMesh.h>
#include <src/world/terrain/NormalGenerationMethod.h>
#include <src/world/terrain/Terrain.h>
#include <src/world/GeometryTypes.h>

struct Planet;

struct TerrainRegion
{
    glm::ivec3 m_Min;
    glm::ivec3 m_Max;
};

struct TerrainProperties
{
    u32 m_ChunksPerEdge;
    f32 m_ChunkSize;
};

struct TerrainMeshUpdateParams
{
    glm::ivec3 m_ZoneCoordinates;
    const Planet* m_Planet = nullptr;
    Terrain m_Terrain;
    std::vector<TerrainChunk> m_ExistingChunks;
    TerrainProperties m_Properties;
    TerrainRegion m_DirtyRegion;
    NormalGenerationMethod m_NormalGenerationMethod = NormalGenerationMethod::FromVolume;
};

class TerrainMeshUpdater
{
public:
    explicit TerrainMeshUpdater(TerrainMeshUpdateParams _params);

    RawMesh GetRawMesh() const { return m_Mesh; };
    std::vector<TerrainChunk> GetChunks() const { return m_Chunks; };

private:
    void UpdateChunks(const TerrainMeshUpdateParams& _params, std::vector<TerrainChunk>& _existingChunks) const;
    void ConvertToRawMesh(const TerrainMeshUpdateParams& _params, const Terrain& _terrain, std::vector<TerrainChunk>& _existingChunks, const TerrainProperties& _properties, NormalGenerationMethod _normalGenerationMethod, RawMesh& _outRawMesh) const;

private:
    RawMesh m_Mesh;
    std::vector<TerrainChunk> m_Chunks;
};
