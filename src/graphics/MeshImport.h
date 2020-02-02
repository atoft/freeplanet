//
// Created by alastair on 02/12/19.
//

#pragma once

#include <vector>
#include <optional>
#include <string>
#include <GL/glew.h>

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
};