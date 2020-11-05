//
// Created by alastair on 02/12/19.
//

#pragma once

#include <vector>
#include <optional>
#include <string>
#include <GL/glew.h>
#include <src/graphics/RawMesh.h>

class MeshImport
{
public:
    struct ImportedMeshData
    {
        std::vector<GLfloat> m_Vertices;
        std::vector<GLuint> m_Elements;
    };

public:
    static std::optional<ImportedMeshData> ImportOBJ(const std::string& _path);
    static RawMesh ConvertToRawMesh(const MeshImport::ImportedMeshData& _importedMesh);
};
