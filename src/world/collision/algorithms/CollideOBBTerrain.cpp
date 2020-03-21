//
// Created by alastair on 21/03/2020.
//

#include "CollideOBBTerrain.h"

#include <src/tools/MathsHelpers.h>
#include <src/world/collision/algorithms/CollideOBBTriangle.h>
#include <src/world/terrain/TerrainComponent.h>

std::optional<CollisionResult> CollideOBBTerrain::Collide(const glm::mat4 &_transform1, const AABB &_bounds1, const TerrainComponent &_terrain, const glm::vec3 &_terrainOffset)
{
    glm::vec3 overallChosenAxis = glm::vec3();
    f32 overallSeparation = std::numeric_limits<f32>::max();
    std::string debug;

    const glm::vec3 relativeZoneOffset = glm::vec3(0.f); // TODO
    const glm::vec3 zonePositionOffset = relativeZoneOffset * glm::vec3(_terrain.m_ChunkSize * _terrain.m_ChunksPerEdge);
    const glm::vec3 chunksToZoneOriginOffset = glm::vec3(_terrain.m_ChunkSize * _terrain.m_ChunksPerEdge) /2.f;

    const glm::vec3 localPosition = MathsHelpers::GetPosition(_transform1) + _bounds1.m_PositionOffset + chunksToZoneOriginOffset + zonePositionOffset;

    // Use the bounds of the collider to only test chunks that could possibly overlap.
    const AABB boundsForOBB = CollisionHelpers::GetAABBForOBB(MathsHelpers::GetRotationMatrix(_transform1), _bounds1);
    const glm::ivec3 minRegion = glm::floor((localPosition - boundsForOBB.m_Dimensions) / _terrain.m_ChunkSize);
    const glm::ivec3 maxRegion = glm::ceil((localPosition + boundsForOBB.m_Dimensions) / _terrain.m_ChunkSize);

    const glm::ivec3 clampedMinRegion = glm::clamp(minRegion, glm::ivec3(), glm::ivec3(_terrain.m_ChunksPerEdge));
    const glm::ivec3 clampedMaxRegion = glm::clamp(maxRegion, glm::ivec3(), glm::ivec3(_terrain.m_ChunksPerEdge));

    const u32 dimensions = _terrain.GetDimensions();
    const CollisionHelpers::OBBProperties obb = CollisionHelpers::GetOBBProperties(_transform1, _bounds1, _terrainOffset);

    for (s32 z = clampedMinRegion.z; z < clampedMaxRegion.z; ++z)
    {
        for (s32 y = clampedMinRegion.y; y < clampedMaxRegion.y; ++y)
        {
            for (s32 x = clampedMinRegion.x; x < clampedMaxRegion.x; ++x)
            {
                const TerrainChunk& chunk = _terrain.GetChunks()[x + y * dimensions + z * dimensions * dimensions];

                for (u32 triIdx = 0; triIdx < chunk.m_Count; ++triIdx)
                {
                    const std::optional<CollisionResult> triangleResult = CollideOBBTriangle::Collide(obb, chunk.m_Triangles[triIdx]);

                    if (triangleResult != std::nullopt)
                    {
                        // When comparing collisions of the separate triangles, take the collision with the greatest
                        // penetration (as opposed to when doing an individual separating axis test, when we choose the
                        // axis with smallest penetration).
                        if (triangleResult->m_Distance < overallSeparation)
                        {
                            overallSeparation = triangleResult->m_Distance;
                            overallChosenAxis = triangleResult->m_Normal;
                            debug = triangleResult->m_DebugInfo;
                        }
                    }
                }
            }
        }
    }

    if (overallSeparation <= 0.f)
    {
        CollisionResult result;
        assert(MathsHelpers::IsNormalized(overallChosenAxis));
        result.m_Normal = overallChosenAxis;
        result.m_Distance = overallSeparation;
        result.m_DebugInfo = debug;
        return result;
    }

    return std::nullopt;
}
