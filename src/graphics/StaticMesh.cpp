//
// Created by alastair on 14/10/18.
//

#include "StaticMesh.h"

#include <src/tools/globals.h>
#include <src/graphics/MeshImport.h>
#include <src/graphics/ShaderProgram.h>

void StaticMesh::AcquireResources(MeshAssetID _meshAsset)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    std::string modelPath = Globals::FREEPLANET_ASSET_PATH + "models/" + Assets::GetMeshAssetName(_meshAsset) + ".obj";
    
    std::optional<MeshImport::ImportedMeshData> mesh = MeshImport::ImportOBJ(modelPath);

    const GLHelpers::VertexDataBitfield vertexDataMask = GLHelpers::VertexData_Position | GLHelpers::VertexData_Normal | GLHelpers::VertexData_TexCoords;

    if (mesh.has_value())
    {
        GLHelpers::LoadToGPU(mesh->m_Vertices.data(), mesh->m_Vertices.size(), mesh->m_Elements.data(), mesh->m_Elements.size(), vertexDataMask, m_Mesh);
    }
    else
    {
        LogError("Couldn't import mesh!");

        // Would rather not abort for this, instead draw a big garbage triangle.

        std::vector<GLfloat> fallbackVerts
        {
            -5.f, -5.f, 0.f,
            0.f, 0.f, 1.f,
            0.f, 0.f,

            5.f, -5.f, 0.f,
            0.f, 0.f, 1.f,
            0.f, 0.f,

            0.f, 5.f, 0.f,
            0.f, 0.f, 1.f,
            0.f, 0.f
        };

        std::vector<GLuint> fallbackElements
        {
            0, 1, 2
        };

        GLHelpers::LoadToGPU(fallbackVerts.data(), fallbackVerts.size(), fallbackElements.data(), fallbackElements.size(), vertexDataMask, m_Mesh);
    }
}

void StaticMesh::ReleaseResources()
{
    GLHelpers::ReleaseFromGPU(m_Mesh);
}
