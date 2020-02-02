//
// Created by alastair on 27/01/2020.
//

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
