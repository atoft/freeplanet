//
// Created by alastair on 04/11/18.
//

// TODO Share implementation from StaticMesh

#include "DynamicMesh.h"

#include <src/graphics/GLHelpers.h>
#include <src/graphics/ShaderProgram.h>
#include <src/world/GeometryTypes.h>

void DynamicMesh::LoadToGPU(const RawMesh& _mesh)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    assert(_mesh.m_Vertices.size() == _mesh.m_Normals.size());
    assert(_mesh.m_Vertices.size() == _mesh.m_Colors.size());

    std::vector<GLfloat> vertices;
    std::vector<GLuint > elements;

    for (u32 vertIdx = 0; vertIdx < _mesh.m_Vertices.size(); ++vertIdx)
    {
        vertices.push_back(_mesh.m_Vertices[vertIdx].x);
        vertices.push_back(_mesh.m_Vertices[vertIdx].y);
        vertices.push_back(_mesh.m_Vertices[vertIdx].z);

        vertices.push_back(_mesh.m_Normals[vertIdx].x);
        vertices.push_back(_mesh.m_Normals[vertIdx].y);
        vertices.push_back(_mesh.m_Normals[vertIdx].z);

        if (!_mesh.m_TextureCoordinates.empty())
        {
            vertices.push_back(_mesh.m_TextureCoordinates[vertIdx].x);
            vertices.push_back(_mesh.m_TextureCoordinates[vertIdx].y);            
        }
        else
        {
            // Placeholder texture coordinates due to terrain not providing any.
            vertices.push_back(0.f);
            vertices.push_back(0.f);
        }
        
        vertices.push_back(_mesh.m_Colors[vertIdx].x);
        vertices.push_back(_mesh.m_Colors[vertIdx].y);
        vertices.push_back(_mesh.m_Colors[vertIdx].z);

        if (!_mesh.m_TerrainSubstance.empty())
        {
            vertices.push_back(_mesh.m_TerrainSubstance[vertIdx].m_Topsoil);
            vertices.push_back(_mesh.m_TerrainSubstance[vertIdx].m_Dirt);
            vertices.push_back(_mesh.m_TerrainSubstance[vertIdx].m_Rock);
            vertices.push_back(_mesh.m_TerrainSubstance[vertIdx].m_Sand);
        }
    }
    static_assert(RawMesh::ms_VertexTypesCount == 5);

    for (const RawTriangle& triangle : _mesh.m_Faces)
    {
        elements.push_back(triangle[0]);
        elements.push_back(triangle[1]);
        elements.push_back(triangle[2]);
    }

    const u32 numberOfVertices = vertices.size();
    const u32 numberOfElements = elements.size();

    GLHelpers::VertexDataBitfield vertexDataMask = GLHelpers::VertexData_Position | GLHelpers::VertexData_Normal | GLHelpers::VertexData_TexCoords | GLHelpers::VertexData_Color;

    if (!_mesh.m_TerrainSubstance.empty())
    {
        vertexDataMask = vertexDataMask | GLHelpers::VertexData_TerrainSubstance;
    }

    if (_mesh.m_SupportInstancing)
    {
        vertexDataMask = vertexDataMask | GLHelpers::VertexData_Inst_Transform;

        // TODO @Performance Need to have separate quad meshes based on whether this is used.
        vertexDataMask = vertexDataMask | GLHelpers::VertexData_Inst_Color;
    }

    GLHelpers::LoadToGPU(vertices.data(), numberOfVertices, elements.data(), numberOfElements, vertexDataMask, m_Mesh);
}

void DynamicMesh::ReleaseFromGPU()
{
    GLHelpers::ReleaseFromGPU(m_Mesh);

    LogMessage("A DynamicMesh was released from the GPU.");
}
