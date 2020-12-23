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

#include <src/graphics/Color.h>
#include <src/tools/globals.h>
#include <src/world/terrain/TerrainSubstance.h>

struct Triangle;

using RawTriangle = std::array<u32, 3>;

struct RawMesh
{
    bool IsEmpty() const { return  m_Vertices.empty(); };

    void Translate(const glm::vec3& _offset);
    void Transform(const glm::mat4& _transform);
    
    // Joins two meshes together. Does not merge identical verts.
    void Append(const RawMesh& _other);

    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_TextureCoordinates;
    std::vector<Color> m_Colors;
    std::vector<TerrainSubstance> m_TerrainSubstance;
    static constexpr u32 ms_VertexTypesCount = 5;

    std::vector<RawTriangle> m_Faces;

    bool m_SupportInstancing = false;
};
