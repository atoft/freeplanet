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

#include <array>
#include <vector>

#include <src/tools/globals.h>
#include <src/world/GeometryTypes.h>

class MarchingCubes
{
public:
    using Cell = std::array<f32, 8>;

    static TerrainChunk GetIsosurface(const glm::vec3& _position, f32 _spacing, const Cell& _cell);

public:
    static std::array<glm::vec3, 8> ms_VertexIndexToCubeOffset;

private:
    static std::array<u16, 256> ms_EdgeTable;
    static std::array<std::array<s32, 16>, 256> ms_TriTable;
    static std::array<std::array<u32, 2>, 12> ms_EdgeToVertexIndexes;
};
