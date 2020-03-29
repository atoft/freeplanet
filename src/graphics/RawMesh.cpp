//
// Created by alastair on 28/09/19.
//

#include "RawMesh.h"

void RawMesh::Translate(const glm::vec3& _offset)
{
    for (glm::vec3& vert : m_Vertices)
    {
        vert += _offset;
    }
}

void RawMesh::Append(const RawMesh& _other)
{
    u32 indexOffset = m_Vertices.size();

    m_Vertices.insert(m_Vertices.end(), _other.m_Vertices.begin(), _other.m_Vertices.end());
    m_Normals.insert(m_Normals.end(), _other.m_Normals.begin(), _other.m_Normals.end());
    m_TextureCoordinates.insert(m_TextureCoordinates.end(), _other.m_TextureCoordinates.begin(), _other.m_TextureCoordinates.end());
    m_Colors.insert(m_Colors.end(), _other.m_Colors.begin(), _other.m_Colors.end());

    for (const RawTriangle& face : _other.m_Faces)
    {
        RawTriangle adjustedFace;
        adjustedFace[0] = face[0] + indexOffset;
        adjustedFace[1] = face[1] + indexOffset;
        adjustedFace[2] = face[2] + indexOffset;

        m_Faces.push_back(adjustedFace);
    }
}
