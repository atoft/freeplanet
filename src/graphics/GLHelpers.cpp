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

#include "GLHelpers.h"

#include <src/assets/ShaderAssets.h>
#include <src/graphics/Scene.h>
#include <src/graphics/ShaderProgram.h>

// Produces a C++11 string from an OpenGL-style string.
std::string GLHelpers::ConvertGLString(const GLubyte* _glstring)
{
    return std::string(reinterpret_cast<const char*>(_glstring));
}

void GLHelpers::ReportProperties()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    if(glGetString(GL_VERSION) == nullptr)
    {
        LogError("Failed to get OpenGL properties. Is there a valid OpenGL context?");
        return;
    }

    LogMessage("Version number: " + ConvertGLString(glGetString(GL_VERSION))
        + "\n\tProvided by: " + ConvertGLString(glGetString(GL_VENDOR))
        + "\n\tRunning on: " + ConvertGLString(glGetString(GL_RENDERER))
        + "\n\tPrimary GLSL version supported: " + ConvertGLString(glGetString(GL_SHADING_LANGUAGE_VERSION))
        + "\n\tPowered by: Sticky tape, rubber bands.");
}

void GLHelpers::ReportError(std::string _location)
{
    static GLenum lastGLError = 0;
    static std::string lastGLErrLocation;
    static bool didRepeat = false;

    //TODO: Disable in release builds
    GLenum glErr = glGetError();

    if (glErr != 0)
    {
        if (didRepeat && lastGLError == glErr && lastGLErrLocation == _location)
        {
            return;
        }

        if (lastGLError == glErr && lastGLErrLocation == _location)
        {
            didRepeat = true;
            LogError(ConvertGLString(gluErrorString(glErr)) + " at \"" + _location + "\" occurred again. Suppressing further errors...");
            return;
        }

        LogError(ConvertGLString(gluErrorString(glErr)) + " at \"" + _location + "\"" );
        lastGLError = glErr;
        lastGLErrLocation = _location;
        didRepeat = false;

        assert(false);
    }
}

void GLHelpers::LoadToGPU(const GLfloat* _vertices, u32 _numberOfVertices, const GLuint* _elements, u32 _numberOfElements, VertexDataBitfield _vertexDataMask, Renderable::Mesh& _outMesh)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    // Create Vertex Array Object
    glGenVertexArrays(1, &_outMesh.m_VaoHandle);
    glBindVertexArray(_outMesh.m_VaoHandle);

    GLHelpers::ReportError("glBindVertexArray");

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &_outMesh.m_VboHandle);

    GLHelpers::ReportError("glGenBuffers vbo");

    glBindBuffer(GL_ARRAY_BUFFER, _outMesh.m_VboHandle);
    GLHelpers::ReportError("glBindBuffer vao");
    glBufferData(GL_ARRAY_BUFFER, _numberOfVertices * sizeof(GLfloat), _vertices, GL_STATIC_DRAW);


    // Create an element array
    glGenBuffers(1, &_outMesh.m_EboHandle);
    GLHelpers::ReportError("glGenBuffers ebo");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _outMesh.m_EboHandle);
    GLHelpers::ReportError("glBindBuffer ebo");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _numberOfElements * sizeof(GLuint), _elements, GL_STATIC_DRAW);

    _outMesh.m_NumberOfElements = _numberOfElements;

    SetupForShader(_vertexDataMask, _outMesh);

    // Unbind this VAO
    glBindVertexArray(0);
}

u32 GetDimensions(GLHelpers::VertexDataBitfield _vertexData)
{
    switch (_vertexData)
    {
        case GLHelpers::VertexData_Position:
            return 3;
        case GLHelpers::VertexData_Normal:
            return 3;
        case GLHelpers::VertexData_TexCoords:
            return 2;
        case GLHelpers::VertexData_Color:
            return 3;
        case GLHelpers::VertexData_TerrainSubstance:
            return 4;
        default:
            assert(false);
            return 0;
    }
}

void GLHelpers::SetupForShader(VertexDataBitfield _vertexDataMask, Renderable::Mesh& _inOutMesh)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    // The below duplicated code could be rolled into a loop.
    // For now it's left separated for clarity.

    u32 dimensions = 0;

    if (_vertexDataMask & VertexData_Position)
    {
        dimensions += GetDimensions(VertexData_Position);
    }

    if (_vertexDataMask & VertexData_Normal)
    {
        dimensions += GetDimensions(VertexData_Normal);
    }

    if (_vertexDataMask & VertexData_TexCoords)
    {
        dimensions += GetDimensions(VertexData_TexCoords);
    }

    if (_vertexDataMask & VertexData_Color)
    {
        dimensions += GetDimensions(VertexData_Color);
    }

    if (_vertexDataMask & VertexData_TerrainSubstance)
    {
        dimensions += GetDimensions(VertexData_TerrainSubstance);
    }

    // Specify the layout of the vertex data.

    u32 offsetIntoVertexData = 0;

    if (_vertexDataMask & VertexData_Position)
    {
        const AttribLocation attrib = AttribLocation::frplPosition;
        BindVertexAttribToVertexData(attrib, GetDimensions(VertexData_Position), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_Normal)
    {
        const AttribLocation attrib = AttribLocation::frplNormal;
        BindVertexAttribToVertexData(attrib, GetDimensions(VertexData_Normal), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_TexCoords)
    {
        const AttribLocation attrib = AttribLocation::frplTexcoord;
        BindVertexAttribToVertexData(attrib, GetDimensions(VertexData_TexCoords), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_Color)
    {
        const AttribLocation attrib = AttribLocation::frplColor;
        BindVertexAttribToVertexData(attrib, GetDimensions(VertexData_Color), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_TerrainSubstance)
    {
        const AttribLocation attrib = AttribLocation::frplTerrainSubstance;
        BindVertexAttribToVertexData(attrib, GetDimensions(VertexData_TerrainSubstance), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_Inst_Transform)
    {
        {
            // Create a buffer to write instance data to during the render loop.
            GLuint instTransformHandle;
            glGenBuffers(1, &instTransformHandle);
            glBindBuffer(GL_ARRAY_BUFFER, instTransformHandle);

            constexpr u32 MATRIX_SIZE = 4;
            for (u32 attribIdx = 0; attribIdx < MATRIX_SIZE ; ++attribIdx)
            {
                glVertexAttribPointer(AttribLocation::frplInstanceTransform + attribIdx, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                                        (const GLvoid*)(sizeof(GLfloat) * attribIdx * 4));
            }

            _inOutMesh.m_InstanceTransformsHandle = instTransformHandle;
        }

        // @Performance Allow instances without normal transforms for cheaper particles.
        {
            // Create a buffer to write instance data to during the render loop.
            GLuint instNormalTransformHandle;
            glGenBuffers(1, &instNormalTransformHandle);
            glBindBuffer(GL_ARRAY_BUFFER, instNormalTransformHandle);

            constexpr u32 MATRIX_SIZE = 4;
            for (u32 attribIdx = 0; attribIdx < MATRIX_SIZE ; ++attribIdx)
            {
                glVertexAttribPointer(AttribLocation::frplInstanceNormalTransform + attribIdx, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                                      (const GLvoid*)(sizeof(GLfloat) * attribIdx * 4));
                GLHelpers::ReportError("glGetAttribPointer instance normal");

            }

            _inOutMesh.m_InstanceNormalTransformsHandle = instNormalTransformHandle;
        }
    }

    if (_vertexDataMask & VertexData_Inst_Color)
    {
        // Create a buffer to write instance data to during the render loop.
        GLuint instColorsHandle;
        glGenBuffers(1, &instColorsHandle);
        glBindBuffer(GL_ARRAY_BUFFER, instColorsHandle);

        glVertexAttribPointer(AttribLocation::frplInstanceColor, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
        GLHelpers::ReportError("glGetAttribPointer instance color");

        _inOutMesh.m_InstanceColorsHandle = instColorsHandle;
    }
}

void GLHelpers::BindVertexAttribToVertexData(AttribLocation _attrib, u32 _numberOfDimensions, u32 _totalVertexDimensions, u32& _inOutOffsetIntoVertex)
{
    if (_attrib >= 0)
    {
        glVertexAttribPointer(
                _attrib,
                _numberOfDimensions,
                GL_FLOAT,
                GL_FALSE,
                _totalVertexDimensions * sizeof(GLfloat),
                reinterpret_cast<GLvoid*>(_inOutOffsetIntoVertex * sizeof(GLfloat)));
        GLHelpers::ReportError("glVertexAttribPointer");
    }

    _inOutOffsetIntoVertex += _numberOfDimensions;
}

void GLHelpers::ReleaseFromGPU(Renderable::Mesh& _inOutMesh)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    glDeleteBuffers(1, &_inOutMesh.m_EboHandle);
    glDeleteBuffers(1, &_inOutMesh.m_VboHandle);
    glDeleteVertexArrays(1, &_inOutMesh.m_VaoHandle);

    _inOutMesh = Renderable::Mesh();
}
