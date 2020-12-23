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

#include "src/graphics/ShaderProgram.h"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <src/tools/globals.h>

namespace Renderable
{
    struct Mesh;
}

class ShaderProgram;

class GLHelpers
{
public:
    static void ReportProperties();
    static void ReportError(std::string _location);

    enum VertexDataBitfield : u8
    {
        VertexData_None             = 0b00000000u,

        VertexData_Position         = 1 << 0,
        VertexData_Normal           = 1 << 1,
        VertexData_TexCoords        = 1 << 2,
        VertexData_Color            = 1 << 3,
        VertexData_TerrainSubstance = 1 << 4,
        VertexData_Inst_Transform   = 1 << 5,
        VertexData_Inst_Color       = 1 << 6,

        VertexData_All              = VertexData_Position | VertexData_Normal | VertexData_TexCoords | VertexData_Color | VertexData_TerrainSubstance | VertexData_Inst_Transform | VertexData_Inst_Color
    };

    static void LoadToGPU(const GLfloat* _vertices, u32 _numberOfVertices, const GLuint* _elements, u32 _numberOfElements, VertexDataBitfield _vertexDataMask, Renderable::Mesh& _outMesh);
    static void ReleaseFromGPU(Renderable::Mesh& _inOutMesh);

private:
    static std::string ConvertGLString(const GLubyte* _glstring);
    static void SetupForShader(VertexDataBitfield _vertexDataMask, Renderable::Mesh& _inOutMesh);
    static void BindVertexAttribToVertexData(AttribLocation _attrib, u32 _numberOfDimensions, u32 _totalVertexDimensions, u32& _inOutOffsetIntoVertex);
};

inline GLHelpers::VertexDataBitfield operator&(GLHelpers::VertexDataBitfield _left, GLHelpers::VertexDataBitfield _right)
{
    return static_cast<GLHelpers::VertexDataBitfield>(static_cast<u32>(_left) & static_cast<u32>(_right));
}

inline GLHelpers::VertexDataBitfield operator|(GLHelpers::VertexDataBitfield _left, GLHelpers::VertexDataBitfield _right)
{
    return static_cast<GLHelpers::VertexDataBitfield>(static_cast<u32>(_left) | static_cast<u32>(_right));
}

