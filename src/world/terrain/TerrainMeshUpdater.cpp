//
// Created by alastair on 08/09/19.
//

#include "TerrainMeshUpdater.h"

#include <algorithm>

#include <src/world/terrain/MarchingCubes.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/planet/TerrainGeneration.h>
#include <src/world/WorldPosition.h>

TerrainMeshUpdater::TerrainMeshUpdater(TerrainMeshUpdateParams _params)
{
    m_Chunks = _params.m_ExistingChunks;
    UpdateChunks(_params, m_Chunks);
    ConvertToRawMesh(_params, m_Chunks, m_Mesh);
}

f32 GetDensity(const TerrainMeshUpdateParams& _params, glm::vec3 _pos)
{
    f32 density = 0.f;

    if (_params.m_Planet != nullptr)
    {
        const glm::vec3 localPosition = TerrainHelpers::ToLocalSpace(_pos, _params.m_Properties);
        density += TerrainGeneration::GetDensity(*_params.m_Planet, {_params.m_ZoneCoordinates, localPosition}, _params.m_LevelOfDetail);
    }

    density += _params.m_TerrainEdits.GetDensity(_pos);

    return TerrainGeneration::ClampDensity(density);
}

TerrainSubstance GetSubstance(const TerrainMeshUpdateParams& _params, glm::vec3 _pos)
{
    TerrainSubstance substance;

    if (_params.m_Planet != nullptr)
    {
        const glm::vec3 localPosition = TerrainHelpers::ToLocalSpace(_pos, _params.m_Properties);
        substance = TerrainGeneration::GetSubstance(*_params.m_Planet, {_params.m_ZoneCoordinates, localPosition}, _params.m_LevelOfDetail);
    }

    _params.m_TerrainEdits.GetSubstance(_pos, substance);

    return substance;
}

void TerrainMeshUpdater::UpdateChunks(const TerrainMeshUpdateParams& _params, std::vector<TerrainChunk>& _existingChunks) const
{
    const f32 spacing = _params.m_Properties.m_ChunkSize;
    const u32 dimensions = _params.m_Properties.m_ChunksPerEdge;
    assert(_existingChunks.size() == dimensions * dimensions * dimensions);

    const TerrainRegion& region = _params.m_DirtyRegion;

    for (s32 z = region.m_Min.z; z < region.m_Max.z; ++z)
    {
        for (s32 y = region.m_Min.y; y < region.m_Max.y; ++y)
        {
            for (s32 x = region.m_Min.x; x < region.m_Max.x; ++x)
            {
                if (_params.m_IsFringe
                    && z > region.m_Min.z && z < region.m_Max.z - 1
                    && y > region.m_Min.y && y < region.m_Max.y - 1
                    && x > region.m_Min.x && x < region.m_Max.x - 1)
                {
                    continue;
                }

                const glm::vec3 pos = glm::vec3(static_cast<f32>(x) * spacing, static_cast<f32>(y) * spacing, static_cast<f32>(z) * spacing);

                MarchingCubes::Cell cell;
                for (u32 cellIdx = 0; cellIdx < 8; ++cellIdx)
                {
                    const f32 density = GetDensity(_params, pos + spacing * MarchingCubes::ms_VertexIndexToCubeOffset[cellIdx]);
                    cell[cellIdx] = density;
                }

                _existingChunks.at(x + y * dimensions + z * dimensions * dimensions) = MarchingCubes::GetIsosurface(pos, spacing, cell);
            }
        }
    }
}

void TerrainMeshUpdater::ConvertToRawMesh(const TerrainMeshUpdateParams& _params, const std::vector<TerrainChunk>& _existingChunks, RawMesh& _outRawMesh) const
{
    const f32 dimensions = _params.m_Properties.m_ChunksPerEdge;

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
                        auto it = std::find(
                                _outRawMesh.m_Vertices.begin(),
                                _outRawMesh.m_Vertices.end(),
                                vert);

                        if (it != _outRawMesh.m_Vertices.end())
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

    switch (_params.m_NormalGenerationMethod)
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
            const f32 gradX = GetDensity(_params, _outRawMesh.m_Vertices[vertIdx] - glm::vec3(1, 0, 0) * GRADIENT_EPSILON)
                              - GetDensity(_params, _outRawMesh.m_Vertices[vertIdx] + glm::vec3(1, 0, 0) * GRADIENT_EPSILON);

            const f32 gradY = GetDensity(_params, _outRawMesh.m_Vertices[vertIdx] - glm::vec3(0, 1, 0) * GRADIENT_EPSILON)
                              - GetDensity(_params, _outRawMesh.m_Vertices[vertIdx] + glm::vec3(0, 1, 0) * GRADIENT_EPSILON);

            const f32 gradZ = GetDensity(_params, _outRawMesh.m_Vertices[vertIdx] - glm::vec3(0, 0, 1) * GRADIENT_EPSILON)
                              - GetDensity(_params, _outRawMesh.m_Vertices[vertIdx] + glm::vec3(0, 0, 1) * GRADIENT_EPSILON);

            _outRawMesh.m_Normals.push_back(glm::normalize(glm::vec3(gradX, gradY, gradZ)));
        }
        break;
    }
    default:
        break;
    }

    if (_params.m_Planet != nullptr)
    {
        for (u32 vertIdx = 0; vertIdx < _outRawMesh.m_Vertices.size(); ++vertIdx)
        {
            const glm::vec3 localPosition = TerrainHelpers::ToLocalSpace(_outRawMesh.m_Vertices[vertIdx], _params.m_Properties);

            _outRawMesh.m_Colors.push_back(TerrainGeneration::GetColor(*_params.m_Planet, {_params.m_ZoneCoordinates, localPosition}));

            _outRawMesh.m_TerrainSubstance.push_back(GetSubstance(_params, _outRawMesh.m_Vertices[vertIdx]));
        }
    }
    else
    {
        for (u32 vertIdx = 0; vertIdx < _outRawMesh.m_Vertices.size(); ++vertIdx)
        {
            _outRawMesh.m_Colors.emplace_back(1.f,1.f,1.f,1.f);
        }
    }
}
