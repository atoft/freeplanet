//
// Created by alastair on 04/11/18.
//

#pragma once

#include <src/graphics/RawMesh.h>
#include <src/graphics/Scene.h>

class ShaderProgram;
struct TerrainChunk;

using DynamicMeshID = u32;
constexpr DynamicMeshID DYNAMICMESHID_INVALID = 0;

class DynamicMesh
{
public:
    DynamicMesh() = default;
    ~DynamicMesh() = default;

    void SetupForShader(const ShaderProgram& _shader);

    const Renderable::Mesh& GetMesh() const { return m_Mesh; };

    void LoadToGPU(const RawMesh& _mesh);
    void ReleaseFromGPU();
private:
    void LoadToGPU(const GLfloat* _vertices, u32 _sizeofVertices, const GLuint* _elements, u32 _sizeofElements);

private:
    Renderable::Mesh m_Mesh;
};
