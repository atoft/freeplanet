//
// Created by alastair on 14/10/18.
//

#include "StaticMesh.h"

#include <src/tools/globals.h>
#include <src/graphics/MeshImport.h>
#include <src/graphics/ShaderProgram.h>

StaticMesh::StaticMesh(MeshAssetID _meshAsset)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    std::string modelPath = Globals::FREEPLANET_ASSET_PATH + "models/" + Assets::GetMeshAssetName(_meshAsset) + ".obj";
    
    std::optional<MeshImport::ImportedMeshData> mesh = MeshImport::ImportOBJ(modelPath);

    if (mesh.has_value())
    {
        LoadToGPU(mesh->m_Vertices.data(), mesh->m_Vertices.size() * sizeof(GLfloat), mesh->m_Elements.data(), mesh->m_Elements.size() * sizeof(GLuint));
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

        LoadToGPU(fallbackVerts.data(), fallbackVerts.size() * sizeof(GLfloat), fallbackElements.data(), fallbackElements.size() * sizeof(GLuint));
    }
}

void StaticMesh::LoadToGPU(const GLfloat* _vertices, u32 _sizeofVertices, const GLuint* _elements, u32 _sizeofElements)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    // Create Vertex Array Object
    glGenVertexArrays(1, &m_Mesh.m_VaoHandle);
    glBindVertexArray(m_Mesh.m_VaoHandle);

    Globals::ReportGLError("glBindVertexArray");

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &m_Mesh.m_VboHandle);

    Globals::ReportGLError("glGenBuffers vbo");

    glBindBuffer(GL_ARRAY_BUFFER, m_Mesh.m_VboHandle);
    Globals::ReportGLError("glBindBuffer vao");
    glBufferData(GL_ARRAY_BUFFER, _sizeofVertices, _vertices, GL_STATIC_DRAW);


    // Create an element array
    glGenBuffers(1, &m_Mesh.m_EboHandle);
    Globals::ReportGLError("glGenBuffers ebo");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Mesh.m_EboHandle);
    Globals::ReportGLError("glBindBuffer ebo");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _sizeofElements, _elements, GL_STATIC_DRAW);

    m_Mesh.m_SizeofElements = _sizeofElements;

    ShaderProgram* defaultShader = AssetHandle<ShaderProgram>(ShaderAsset_Default).GetAsset();
    assert(defaultShader != nullptr);
    SetupForShader(*defaultShader);

    //Unbind this VAO
    glBindVertexArray(0);
}

void StaticMesh::SetupForShader(const ShaderProgram& _shader)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    constexpr u32 dimensions = (3 + 3 + 2);

    // Specify the layout of the vertex data

    // position
    GLint posAttrib = glGetAttribLocation(_shader.GetProgramHandle(), "frplPosition");
    Globals::ReportGLError("glGetAttribLocation position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, dimensions * sizeof(GLfloat), 0);

    // normal
    GLint normalAttrib = glGetAttribLocation(_shader.GetProgramHandle(), "frplNormal");
    Globals::ReportGLError("glGetAttribLocation normal");
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, dimensions * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    Globals::ReportGLError("glVertexAttribPointer normal");

    // texcoords
    GLint texcoordAttrib = glGetAttribLocation(_shader.GetProgramHandle(), "frplTexcoord");
    Globals::ReportGLError("glGetAttribLocation texcoord");
    if(texcoordAttrib >= 0)
    {
        glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, dimensions * sizeof(GLfloat),
                              (GLvoid *) (6 * sizeof(GLfloat)));
        Globals::ReportGLError("glVertexAttribPointer texcoord");
    }

    if(posAttrib < 0 || normalAttrib < 0 || texcoordAttrib <0)
    {
        std::string errmsg = "Warning: the following variable(s) were not used in the shader "
                             "(they may have been optimised out): \n\t[";

        if(posAttrib < 0)
        {
            errmsg += "frplPosition ";
        }
        if(normalAttrib < 0)
        {
            errmsg += "frplNormal ";
        }
        if(texcoordAttrib < 0)
        {
            errmsg += "frplTexcoord ";
        }

        LogWarning(errmsg + "] \n\tThis will result in OpenGL errors.");
    }

    m_Mesh.m_VertexAttribs = { posAttrib, normalAttrib, texcoordAttrib };
}

StaticMesh::~StaticMesh()
{
     assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

     glDeleteBuffers(1, &m_Mesh.m_EboHandle);
     glDeleteBuffers(1, &m_Mesh.m_VboHandle);
     glDeleteVertexArrays(1, &m_Mesh.m_VaoHandle);
}
