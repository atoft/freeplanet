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

#include "TerrainComponent.h"

TerrainComponent::TerrainComponent(u32 _chunksPerEdge, f32 _chunkSize, glm::ivec3 _zoneCoords)
{
    m_TerrainChunks = std::vector<TerrainChunk>(_chunksPerEdge * _chunksPerEdge * _chunksPerEdge);
    m_Properties.m_ChunksPerEdge = _chunksPerEdge;
    m_ZoneCoordinates = _zoneCoords;
    m_Properties.m_ChunkSize = _chunkSize;
    SetAllDirty();
}

void TerrainComponent::SetAllDirty()
{
   m_DirtyRegion = {glm::ivec3(), glm::ivec3(m_Properties.m_ChunksPerEdge)};
}

TerrainComponent::TerrainComponent()
{
    m_Properties.m_ChunksPerEdge = 0;
    m_Properties.m_ChunkSize = 0.f;
}
