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


#include "TerrainHelpers.h"

glm::vec3 TerrainHelpers::ToTerrainMeshSpace(glm::vec3 _localPosition, const TerrainProperties& _properties)
{
    const glm::vec3 chunksToZoneOriginOffset = glm::vec3(_properties.m_ChunkSize * static_cast<f32>(_properties.m_ChunksPerEdge)) /2.f;
    const glm::vec3 meshSpacePosition = _localPosition + chunksToZoneOriginOffset;

    return meshSpacePosition;
}

glm::vec3 TerrainHelpers::ToLocalSpace(glm::vec3 _terrainMeshSpacePosition, const TerrainProperties& _properties)
{
    const glm::vec3 chunksToZoneOriginOffset = glm::vec3(_properties.m_ChunkSize * static_cast<f32>(_properties.m_ChunksPerEdge)) /2.f;
    const glm::vec3 localPosition = _terrainMeshSpacePosition - chunksToZoneOriginOffset;

    return localPosition;
}
