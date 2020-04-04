//
// Created by alastair on 24/02/18.
//

#include "TerrainComponent.h"

TerrainComponent::TerrainComponent(u32 _chunksPerEdge, f32 _chunkSize, glm::ivec3 _zoneCoords)
{
    m_TerrainChunks = std::vector<TerrainChunk>(_chunksPerEdge * _chunksPerEdge * _chunksPerEdge);
    m_ChunksPerEdge = _chunksPerEdge;
    m_ZoneCoordinates = _zoneCoords;
    m_ChunkSize = _chunkSize;
    m_DirtyRegion = {glm::ivec3(), glm::ivec3(m_ChunksPerEdge, m_ChunksPerEdge, m_ChunksPerEdge)};

    m_Terrain.m_GlobalPositionOffset = glm::vec3(_zoneCoords) * _chunkSize * glm::vec3(_chunksPerEdge);;
}

TerrainComponent::TerrainComponent()
{
    m_ChunksPerEdge = 0;
    m_ChunkSize = 0.f;
}
