//
// Created by alastair on 17/02/2020.
//

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

    ShaderProgram* defaultShader = AssetHandle<ShaderProgram>(ShaderAsset_Default).GetAsset();
    assert(defaultShader != nullptr);
    SetupForShader(*defaultShader, _vertexDataMask, _outMesh);

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

void GLHelpers::SetupForShader(const ShaderProgram& _shader, VertexDataBitfield _vertexDataMask, Renderable::Mesh& _inOutMesh)
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
        const GLint attrib = BindVertexAttribToVertexData(_shader, "frplPosition", GetDimensions(VertexData_Position), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_Normal)
    {
        const GLint attrib = BindVertexAttribToVertexData(_shader, "frplNormal", GetDimensions(VertexData_Normal), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_TexCoords)
    {
        const GLint attrib = BindVertexAttribToVertexData(_shader, "frplTexcoord", GetDimensions(VertexData_TexCoords), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_Color)
    {
        const GLint attrib = BindVertexAttribToVertexData(_shader, "frplColor", GetDimensions(VertexData_Color), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_TerrainSubstance)
    {
        const GLint attrib = BindVertexAttribToVertexData(_shader, "frplTerrainSubstance", GetDimensions(VertexData_TerrainSubstance), dimensions, offsetIntoVertexData);
        _inOutMesh.m_VertexAttribs.push_back(attrib);
    }

    if (_vertexDataMask & VertexData_Inst_Transform)
    {
        GLint attribLocation = glGetAttribLocation(_shader.GetProgramHandle(), "frplInstanceTransform");
        GLHelpers::ReportError("glGetAttribLocation");
        glBindBuffer(GL_ARRAY_BUFFER, attribLocation);

        constexpr u32 MATRIX_SIZE = 4;
        for (u32 attribIdx = 0; attribIdx < MATRIX_SIZE ; ++attribIdx)
        {
            glEnableVertexAttribArray(attribLocation + attribIdx);
            glVertexAttribPointer(attribLocation + attribIdx, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                                    (const GLvoid*)(sizeof(GLfloat) * attribIdx * 4));
            glVertexAttribDivisor(attribLocation + attribIdx, 1);
        }
    }
}

GLint GLHelpers::BindVertexAttribToVertexData(const ShaderProgram& _shader, const char* _name, u32 _numberOfDimensions, u32 _totalVertexDimensions, u32& _inOutOffsetIntoVertex)
{
    GLint attribLocation = glGetAttribLocation(_shader.GetProgramHandle(), _name);
    GLHelpers::ReportError("glGetAttribLocation");
    if (attribLocation >= 0)
    {
        glVertexAttribPointer(
                attribLocation,
                _numberOfDimensions,
                GL_FLOAT,
                GL_FALSE,
                _totalVertexDimensions * sizeof(GLfloat),
                reinterpret_cast<GLvoid*>(_inOutOffsetIntoVertex * sizeof(GLfloat)));
        GLHelpers::ReportError("glVertexAttribPointer");
    }

    _inOutOffsetIntoVertex += _numberOfDimensions;

    return attribLocation;
}

void GLHelpers::ReleaseFromGPU(Renderable::Mesh& _inOutMesh)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    glDeleteBuffers(1, &_inOutMesh.m_EboHandle);
    glDeleteBuffers(1, &_inOutMesh.m_VboHandle);
    glDeleteVertexArrays(1, &_inOutMesh.m_VaoHandle);

    _inOutMesh = Renderable::Mesh();
}
