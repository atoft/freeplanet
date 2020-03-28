//
// Created by alastair on 14/10/18.
//


#pragma once

#include <src/assets/MeshAssets.h>
#include <src/assets/ShaderAssets.h>
#include <src/graphics/Scene.h>

// A StaticMesh is a mesh whose content is statically determined ie. not created at runtime
class StaticMesh
{
public:
    StaticMesh() = default;
    ~StaticMesh() = default;

    void AcquireResources(MeshAssetID _asset);
    void ReleaseResources();

    void SetupForShader(const ShaderProgram& _shader);

    const Renderable::Mesh& GetMesh() const { return m_Mesh; };
private:
    void LoadToGPU(const GLfloat* _vertices, u32 _sizeofVertices, const GLuint* _elements, u32 _sizeofElements);

private:
    Renderable::Mesh m_Mesh;
};



