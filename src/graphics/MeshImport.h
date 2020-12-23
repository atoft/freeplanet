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
