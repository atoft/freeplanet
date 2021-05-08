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

#include "StaticMesh.h"
#include "src/graphics/GLHelpers.h"

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
        LogError("Couldn't import mesh " + Assets::GetMeshAssetName(_meshAsset) + ".");

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
