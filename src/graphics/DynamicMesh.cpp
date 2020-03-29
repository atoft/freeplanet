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

        // Placeholder texture coordinates
        vertices.push_back(0.f);
        vertices.push_back(0.f);

        vertices.push_back(_mesh.m_Colors[vertIdx].x);
        vertices.push_back(_mesh.m_Colors[vertIdx].y);
        vertices.push_back(_mesh.m_Colors[vertIdx].z);
    }

    for (const RawTriangle& triangle : _mesh.m_Faces)
    {
        elements.push_back(triangle[0]);
        elements.push_back(triangle[1]);
        elements.push_back(triangle[2]);
    }

    const u32 sizeofVertices = (int)vertices.size() * sizeof(GLfloat);
    const u32 sizeofElements = (int)elements.size() * sizeof(GLuint);
    LoadToGPU(vertices.data(), sizeofVertices, elements.data(), sizeofElements);
}

void DynamicMesh::ReleaseFromGPU()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    glDeleteBuffers(1, &m_Mesh.m_EboHandle);
    glDeleteBuffers(1, &m_Mesh.m_VboHandle);
    glDeleteVertexArrays(1, &m_Mesh.m_VaoHandle);

    m_Mesh = Renderable::Mesh();

    LogMessage("A DynamicMesh was released from the GPU.");
}

void DynamicMesh::LoadToGPU(const GLfloat* _vertices, u32 _sizeofVertices, const GLuint* _elements, u32 _sizeofElements)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    // Create Vertex Array Object
    glGenVertexArrays(1, &m_Mesh.m_VaoHandle);
    glBindVertexArray(m_Mesh.m_VaoHandle);

    GLHelpers::ReportError("glBindVertexArray");

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &m_Mesh.m_VboHandle);

    GLHelpers::ReportError("glGenBuffers vbo");

    glBindBuffer(GL_ARRAY_BUFFER, m_Mesh.m_VboHandle);
    GLHelpers::ReportError("glBindBuffer vao");
    glBufferData(GL_ARRAY_BUFFER, _sizeofVertices, _vertices, GL_STATIC_DRAW);


    // Create an element array
    glGenBuffers(1, &m_Mesh.m_EboHandle);
    GLHelpers::ReportError("glGenBuffers ebo");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Mesh.m_EboHandle);
    GLHelpers::ReportError("glBindBuffer ebo");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _sizeofElements, _elements, GL_STATIC_DRAW);

    m_Mesh.m_SizeofElements = _sizeofElements;

    ShaderProgram* defaultShader = AssetHandle<ShaderProgram>(ShaderAsset_Default).GetAsset();
    assert(defaultShader != nullptr);
    SetupForShader(*defaultShader);

    //Unbind this VAO
    glBindVertexArray(0);
}

void DynamicMesh::SetupForShader(const ShaderProgram& _shader)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    constexpr u32 dimensions = (3 + 3 + 2 + 3);

    // Specify the layout of the vertex data.

    // Position
    GLint posAttrib = glGetAttribLocation(_shader.GetProgramHandle(), "frplPosition");
    GLHelpers::ReportError("glGetAttribLocation position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, dimensions * sizeof(GLfloat), 0);

    // Normal
    GLint normalAttrib = glGetAttribLocation(_shader.GetProgramHandle(), "frplNormal");
    GLHelpers::ReportError("glGetAttribLocation normal");
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, dimensions * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    GLHelpers::ReportError("glVertexAttribPointer normal");

    // Texcoords
    GLint texcoordAttrib = glGetAttribLocation(_shader.GetProgramHandle(), "frplTexcoord");
    GLHelpers::ReportError("glGetAttribLocation texcoord");
    if(texcoordAttrib >= 0)
    {
        glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, dimensions * sizeof(GLfloat),
                              (GLvoid *) (6 * sizeof(GLfloat)));
        GLHelpers::ReportError("glVertexAttribPointer texcoord");
    }

    // Color
    GLint colorAttrib = glGetAttribLocation(_shader.GetProgramHandle(), "frplColor");
    GLHelpers::ReportError("glGetAttribLocation Color");
    if (colorAttrib >= 0)
    {
        glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, dimensions * sizeof(GLfloat),
                              (GLvoid *) (8 * sizeof(GLfloat)));
        GLHelpers::ReportError("glVertexAttribPointer Color");
    }

    if (posAttrib < 0 || normalAttrib < 0 || texcoordAttrib < 0 || colorAttrib < 0)
    {
        std::string errmsg = "Warning: the following variable(s) were not used in the shader "
                             "(they may have been optimised out): \n\t[";

        if (posAttrib < 0)
        {
            errmsg += "frplPosition ";
        }
        if (normalAttrib < 0)
        {
            errmsg += "frplNormal ";
        }
        if (texcoordAttrib < 0)
        {
            errmsg += "frplTexcoord ";
        }
        if (colorAttrib < 0)
        {
            errmsg += "frplColor ";
        }

        LogWarning(errmsg + "] \n\tThis will result in OpenGL errors.");
    }

    m_Mesh.m_VertexAttribs = { posAttrib, normalAttrib, texcoordAttrib, colorAttrib };
}
