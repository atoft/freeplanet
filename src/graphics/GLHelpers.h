//
// Created by alastair on 17/02/2020.
//

#pragma once

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
        VertexData_Position    = 0b0001u,
        VertexData_Normal      = 0b0010u,
        VertexData_TexCoords   = 0b0100u,
        VertexData_Color       = 0b1000u,

        VertexData_All         = VertexData_Position | VertexData_Normal | VertexData_TexCoords | VertexData_Color
    };

    static void LoadToGPU(const GLfloat* _vertices, u32 _numberOfVertices, const GLuint* _elements, u32 _numberOfElements, VertexDataBitfield _vertexDataMask, Renderable::Mesh& _outMesh);
    static void ReleaseFromGPU(Renderable::Mesh& _inOutMesh);

private:
    static std::string ConvertGLString(const GLubyte* _glstring);
    static void SetupForShader(const ShaderProgram& _shader, VertexDataBitfield _vertexDataMask, Renderable::Mesh& _inOutMesh);
    static GLint BindVertexAttribToVertexData(const ShaderProgram& _shader, const char* _name, u32 _numberOfDimensions, u32 _totalVertexDimensions, u32& _inOutOffsetIntoVertex);
};

inline GLHelpers::VertexDataBitfield operator&(GLHelpers::VertexDataBitfield _left, GLHelpers::VertexDataBitfield _right)
{
    return static_cast<GLHelpers::VertexDataBitfield>(static_cast<u32>(_left) & static_cast<u32>(_right));
}

inline GLHelpers::VertexDataBitfield operator|(GLHelpers::VertexDataBitfield _left, GLHelpers::VertexDataBitfield _right)
{
    return static_cast<GLHelpers::VertexDataBitfield>(static_cast<u32>(_left) | static_cast<u32>(_right));
}

