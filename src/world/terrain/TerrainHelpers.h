/* 
 * Copyright 2020 Alastair Toft
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <src/tools/globals.h>

struct TerrainProperties
{
    u32 m_ChunksPerEdge;
    f32 m_ChunkSize;
};

class TerrainHelpers
{
public:
    static glm::vec3 ToTerrainMeshSpace(glm::vec3 _localPosition, const TerrainProperties& _properties);
    static glm::vec3 ToLocalSpace(glm::vec3 _terrainMeshSpacePosition, const TerrainProperties& _properties);
};

