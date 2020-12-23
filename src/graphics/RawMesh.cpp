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

#include "RawMesh.h"

#include <glm/geometric.hpp>
#include <src/tools/MathsHelpers.h>

void RawMesh::Translate(const glm::vec3& _offset)
{
    for (glm::vec3& vert : m_Vertices)
    {
        vert += _offset;
    }
}

void RawMesh::Transform(const glm::mat4& _transform)
{
    for (glm::vec3& vert : m_Vertices)
    {
        const glm::vec4 vert4 = glm::vec4(vert.x, vert.y, vert.z, 1.f);

        const glm::vec4 transformed = _transform * vert4;

        vert = transformed;
    }

    glm::mat4 rotationScale = _transform;
    MathsHelpers::SetPosition(rotationScale, glm::vec3(0.f));

    for (glm::vec3& normal : m_Normals)
    {
        const glm::vec4 normal4 = glm::vec4(normal.x, normal.y, normal.z, 1.f);

        const glm::vec4 transformed = rotationScale * normal4;

        normal = transformed;
        normal = glm::normalize(normal);
    }
}


void RawMesh::Append(const RawMesh& _other)
{
    u32 indexOffset = m_Vertices.size();

    m_Vertices.insert(m_Vertices.end(), _other.m_Vertices.begin(), _other.m_Vertices.end());
    m_Normals.insert(m_Normals.end(), _other.m_Normals.begin(), _other.m_Normals.end());
    m_TextureCoordinates.insert(m_TextureCoordinates.end(), _other.m_TextureCoordinates.begin(), _other.m_TextureCoordinates.end());
    m_Colors.insert(m_Colors.end(), _other.m_Colors.begin(), _other.m_Colors.end());
    m_TerrainSubstance.insert(m_TerrainSubstance.end(), _other.m_TerrainSubstance.begin(), _other.m_TerrainSubstance.end());
    static_assert(ms_VertexTypesCount == 5);

    for (const RawTriangle& face : _other.m_Faces)
    {
        RawTriangle adjustedFace;
        adjustedFace[0] = face[0] + indexOffset;
        adjustedFace[1] = face[1] + indexOffset;
        adjustedFace[2] = face[2] + indexOffset;

        m_Faces.push_back(adjustedFace);
    }
}
