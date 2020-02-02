//
// Created by alastair on 08/09/19.
//

#include "TerrainMeshUpdater.h"

#include <algorithm>

#include <src/world/terrain/MarchingCubes.h>
#include <src/tools/MathsHelpers.h>

TerrainMeshUpdater::TerrainMeshUpdater(TerrainMeshUpdateParams _params)
{
    m_Chunks = _params.m_ExistingChunks;
    UpdateChunks(_params.m_Terrain, m_Chunks, _params.m_DirtyRegion, _params.m_Properties);
    ConvertToRawMesh(_params.m_Terrain, m_Chunks, _params.m_Properties, _params.m_NormalGenerationMethod, m_Mesh);
}

void TerrainMeshUpdater::UpdateChunks(const Terrain& _terrain, std::vector<TerrainChunk>& _existingChunks, const TerrainRegion& _region, const TerrainProperties& _properties) const
{
    const f32 spacing = _properties.m_ChunkSize;
    const u32 dimensions = _properties.m_ChunksPerEdge;
    assert(_existingChunks.size() == dimensions * dimensions * dimensions);

    for (s32 z = _region.m_Min.z; z < _region.m_Max.z; ++z)
    {
        for (s32 y = _region.m_Min.y; y < _region.m_Max.y; ++y)
        {
            for (s32 x = _region.m_Min.x; x < _region.m_Max.x; ++x)
            {
                const glm::vec3 pos = glm::vec3(static_cast<f32>(x) * spacing, static_cast<f32>(y) * spacing, static_cast<f32>(z) * spacing);

                MarchingCubes::Cell cell;
                for (u32 cellIdx = 0; cellIdx < 8; ++cellIdx)
                {
                    cell[cellIdx] = _terrain.GetDensity(pos + spacing * MarchingCubes::ms_VertexIndexToCubeOffset[cellIdx]);
                }

                _existingChunks.at(x + y * dimensions + z * dimensions * dimensions) = MarchingCubes::GetIsosurface(pos, spacing, cell);
            }
        }
    }
}

void TerrainMeshUpdater::ConvertToRawMesh(
        const Terrain& _terrain,
        std::vector<TerrainChunk>& _existingChunks,
        const TerrainProperties& _properties,
        NormalGenerationMethod _normalGenerationMethod,
        RawMesh& _outRawMesh) const
{
    const f32 dimensions = _properties.m_ChunksPerEdge;

    // First, we go from triangles to shared vertices and indexes.
    // This significantly reduces the amount of data sent to the GPU.
    for (u32 z = 0; z < dimensions; ++z)
    {
        for (u32 y = 0; y < dimensions; ++y)
        {
            for (u32 x = 0; x < dimensions; ++x)
            {
                const TerrainChunk& chunk = _existingChunks.at(x + y * dimensions + z * dimensions * dimensions);

                for (u32 triIdx = 0; triIdx < chunk.m_Count; ++triIdx)
                {
                    const Triangle& triangle = chunk.m_Triangles[triIdx];
                    RawTriangle& outTriangle = _outRawMesh.m_Faces.emplace_back();

                    u32 triangleVertexIndex = 0;
                    for (const glm::vec3& vert : triangle.m_Vertices)
                    {
                        const auto& it = std::find(
                                _outRawMesh.m_Vertices.begin(),
                                _outRawMesh.m_Vertices.end(),
                                vert);

                        if(it != _outRawMesh.m_Vertices.end())
                        {
                            // An identical vertex has been added before, use the index of that one.
                            const u32 vertIdx = std::distance(_outRawMesh.m_Vertices.begin(), it);

                            outTriangle[triangleVertexIndex] = vertIdx;
                        }
                        else
                        {
                            // This is a new vertex, add it.

                            _outRawMesh.m_Vertices.push_back(vert);

                            const u32 vertIdx = _outRawMesh.m_Vertices.size() - 1;
                            outTriangle[triangleVertexIndex] = vertIdx;
                        }

                        ++triangleVertexIndex;
                    }
                }
            }
        }
    }

    switch (_normalGenerationMethod)
    {
    case NormalGenerationMethod::FromFaceNormals:
    {
        // This is the standard method for computing vertex normals, based on the face normals of the generated
        // triangles. However, marching cubes generates some very small, or very thin, triangles which end up creating
        // visible sharp changes in the normal across the surface, as there's a big difference in normals between very
        // nearby vertices. See https://stackoverflow.com/questions/35112181/marching-cubes-very-small-triangles as an
        // example. We could do some post-processing of the mesh or the normals to smooth out these issues, but for now
        // just calculating a gradient of the volume itself will do, as in the method below.

        // We compute and cache the normals * the face area at each face
        std::vector<glm::vec3> faceNormals;

        for (const RawTriangle& triangle : _outRawMesh.m_Faces)
        {
            faceNormals.push_back(
                    MathsHelpers::ComputeFaceNormal(
                            _outRawMesh.m_Vertices[triangle[0]],
                            _outRawMesh.m_Vertices[triangle[1]],
                            _outRawMesh.m_Vertices[triangle[2]])
            );
        }


        // Finally, we do a pass to determine each vertex normal, based on the normals of triangles
        // containing that vertex.
        for (u32 vertIdx = 0; vertIdx < _outRawMesh.m_Vertices.size(); ++vertIdx)
        {
            std::vector<glm::vec3> normalsAroundVertex;

            u32 faceIdx = 0;
            for (const RawTriangle& triangle : _outRawMesh.m_Faces)
            {
                const auto& it = std::find(triangle.begin(), triangle.end(), vertIdx);

                if (it != triangle.end())
                {
                    // Found one of the triangles with this vertex
                    normalsAroundVertex.push_back(faceNormals[faceIdx]);
                }

                ++faceIdx;
            }

            glm::vec3 resultNormal = glm::vec3();
            u32 normalCount = 0;
            for (const glm::vec3& normal : normalsAroundVertex)
            {
                resultNormal += normal;

                if (resultNormal != glm::vec3())
                {
                    ++normalCount;
                }
            }
            resultNormal /= normalCount;

            _outRawMesh.m_Normals.push_back(glm::normalize(resultNormal));
        }

        break;
    }
    case NormalGenerationMethod::FromVolume:
    {
        constexpr f32 GRADIENT_EPSILON = 0.125f;

        for (u32 vertIdx = 0; vertIdx < _outRawMesh.m_Vertices.size(); ++vertIdx)
        {
            const f32 gradX = _terrain.GetDensity(_outRawMesh.m_Vertices[vertIdx] - glm::vec3(1, 0, 0) * GRADIENT_EPSILON)
                              - _terrain.GetDensity(_outRawMesh.m_Vertices[vertIdx] + glm::vec3(1, 0, 0) * GRADIENT_EPSILON);

            const f32 gradY = _terrain.GetDensity(_outRawMesh.m_Vertices[vertIdx] - glm::vec3(0, 1, 0) * GRADIENT_EPSILON)
                              - _terrain.GetDensity(_outRawMesh.m_Vertices[vertIdx] + glm::vec3(0, 1, 0) * GRADIENT_EPSILON);

            const f32 gradZ = _terrain.GetDensity(_outRawMesh.m_Vertices[vertIdx] - glm::vec3(0, 0, 1) * GRADIENT_EPSILON)
                              - _terrain.GetDensity(_outRawMesh.m_Vertices[vertIdx] + glm::vec3(0, 0, 1) * GRADIENT_EPSILON);

            _outRawMesh.m_Normals.push_back(glm::normalize(glm::vec3(gradX, gradY, gradZ)));
        }
        break;
    }
    default:
        break;
    }
}
