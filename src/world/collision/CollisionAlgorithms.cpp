//
// Created by alastair on 11/02/19.
//

#include "CollisionAlgorithms.h"

#include <vector>
#include <optional>

#include <src/tools/MathsHelpers.h>
#include <src/world/collision/CollisionHelpers.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/terrain/TerrainComponent.h>

std::optional<CollisionResult> CollisionAlgorithms::CollideOBB_OBB(
        const glm::mat4& _transform1,
        const AABB& _bounds1,
        const glm::mat4& _transform2,
        const AABB& _bounds2)
{
    CollisionHelpers::OBBProperties obb1Properties = CollisionHelpers::GetOBBProperties(_transform1, _bounds1, glm::vec3());
    const glm::vec3& obb1X = obb1Properties.m_X;
    const glm::vec3& obb1Y = obb1Properties.m_Y;
    const glm::vec3& obb1Z = obb1Properties.m_Z;
    const std::array<glm::vec3, 8>& obb1Coordinates = obb1Properties.m_Coordinates;

    CollisionHelpers::OBBProperties obb2Properties = CollisionHelpers::GetOBBProperties(_transform2, _bounds2, glm::vec3());
    const glm::vec3& obb2X = obb2Properties.m_X;
    const glm::vec3& obb2Y = obb2Properties.m_Y;
    const glm::vec3& obb2Z = obb2Properties.m_Z;
    const std::array<glm::vec3, 8>& obb2Coordinates = obb2Properties.m_Coordinates;

    glm::vec3 collidingAxis = glm::vec3();
    float minT = -std::numeric_limits<float>::max();

    // Collision detection in axes of OBB1  x3
    if(   !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, obb1X, minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, obb1Y, minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, obb1Z, minT, collidingAxis))
    {
        return std::nullopt;
    }

    // Collision detection in axes of OBB2  x3
    if(   !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, obb2X, minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, obb2Y, minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, obb2Z, minT, collidingAxis))
    {
        return std::nullopt;
    }

    // Collision detection in axes of cross products of edges of OBB1 and OBB2  x9
    if(	  !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1X, obb2X), minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1X, obb2Y), minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1X, obb2Z), minT, collidingAxis)

       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1Y, obb2X), minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1Y, obb2Y), minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1Y, obb2Z), minT, collidingAxis)

       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1Z, obb2X), minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1Z, obb2Y), minT, collidingAxis)
       || !CollisionHelpers::IsOBBCollisionInAxis(obb1Coordinates, obb2Coordinates, CollisionHelpers::TryNormalizedCross(obb1Z, obb2Z), minT, collidingAxis))
    {
        return std::nullopt;
    }

    CollisionResult result;
    result.m_Normal = collidingAxis;
    result.m_Distance = minT;

    return result;
}

std::optional<CollisionResult> CollisionAlgorithms::CollideOBB_Terrain(const glm::mat4& _transform1,
                                             const AABB& _bounds1,
                                             const TerrainComponent& _terrain,
                                             const glm::vec3& _terrainOffset)
{
    glm::vec3 overallChosenAxis = glm::vec3();
    f32 overallSeparation = 0.f;
    bool didCollide = false;
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

    for (s32 z = clampedMinRegion.z; z < clampedMaxRegion.z; ++z)
    {
        for (s32 y = clampedMinRegion.y; y < clampedMaxRegion.y; ++y)
        {
            for (s32 x = clampedMinRegion.x; x < clampedMaxRegion.x; ++x)
            {
                const TerrainChunk& chunk = _terrain.GetChunks()[x + y * dimensions + z * dimensions * dimensions];

                for (u32 triIdx = 0; triIdx < chunk.m_Count; ++triIdx)
                {
                    const std::optional<CollisionResult> triangleResult = CollideOBB_Triangle(_transform1, _bounds1, chunk.m_Triangles[triIdx], _terrainOffset);

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
                            didCollide = true;
                        }
                    }
                }
            }
        }
    }

    if(didCollide && overallSeparation <= 0.f)
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

bool TestAxis(
        const glm::vec3& _axis,
        const CollisionHelpers::OBBProperties _obb,
        const Triangle& _triangle,
        glm::vec3& _inOutAxis,
        f32& _inOutPenetration, std::string& _outDebugInfo)
{
    f32 obbMax = -std::numeric_limits<f32>::max();
    f32 obbMin = std::numeric_limits<f32>::max();

    for (const glm::vec3& obbPoint : _obb.m_Coordinates)
    {
        const f32 distanceOnAxis = glm::dot(obbPoint, _axis);

        obbMax = glm::max(obbMax, distanceOnAxis);
        obbMin = glm::min(obbMin, distanceOnAxis);
    }

    f32 triMax = -std::numeric_limits<f32>::max();
    f32 triMin = std::numeric_limits<f32>::max();

    for (const glm::vec3 triPoint : _triangle.m_Vertices)
    {
        const f32 distanceOnAxis = glm::dot(triPoint, _axis);

        triMax = glm::max(triMax, distanceOnAxis);
        triMin = glm::min(triMin, distanceOnAxis);
    }

    const f32 obbRange = obbMax - obbMin;
    const f32 triRange = triMax - triMin;

    if (glm::epsilonEqual(triMax, triMin, glm::epsilon<f32>()))
    {
        // The triangle normal is closely aligned to _axis.
        // Skip this test as the penetration in the normal direction will
        // be handled in TestTriangleNormalAxis.
        LogMessage("Skipped due to normal aligned with triangle.")
        return true;
    }

    const f32 span = obbRange + triRange;
    const f32 range = glm::max(obbMax, triMax) - glm::min(obbMin, triMin);

    if (range <= span)
    {
        const f32 penetration = span - range;

        const f32 obbCenter = obbMin + (obbMax - obbMin) / 2.f;
        const f32 triCenter = triMin + (triMax - triMin) / 2.f;

        // Always want a normal pointed towards the intersecting triangle.
        const glm::vec3 direction = obbCenter < triCenter ? _axis : -_axis;

        assert(penetration >= 0.f);
        if (penetration < _inOutPenetration)
        {
            _inOutPenetration = penetration;
            _inOutAxis = direction;
            _outDebugInfo = "Axis";
        }

        return true;
    }

    return false;
}

bool TestCrossProductAxis(
        const glm::vec3& _axisA,
        const glm::vec3& _axisB,
        const CollisionHelpers::OBBProperties _obb,
        const Triangle& _triangle,
        glm::vec3& _inOutAxis,
        f32& _inOutPenetration, std::string& _outDebugInfo)
{
    if (glm::abs(glm::dot(_axisA, _axisB)) > 0.99f)
    {
        // The axes are close to parallel, so skip this test.
        return true;
    }

    const glm::vec3 testAxis = glm::normalize(glm::cross(_axisA, _axisB));

    const f32 oldPenetration = _inOutPenetration;

    const bool result = TestAxis(testAxis, _obb, _triangle, _inOutAxis, _inOutPenetration, _outDebugInfo);

    if (oldPenetration != _inOutPenetration)
    {
        _outDebugInfo = "Cross Product Axis";
    }

    return result;
}

bool TestTriangleNormalAxis(
        const CollisionHelpers::OBBProperties _obb,
        const Triangle& _triangle,
        glm::vec3& _inOutAxis,
        f32& _inOutPenetration, std::string& _outDebugInfo)
{
    const glm::vec3 axis = glm::normalize(
            glm::cross(
                    glm::normalize(_triangle.m_Vertices[1] - _triangle.m_Vertices[0]),
                    glm::normalize(_triangle.m_Vertices[2] - _triangle.m_Vertices[1])));

    f32 obbMax = -std::numeric_limits<f32>::max();
    f32 obbMin = std::numeric_limits<f32>::max();

    for (const glm::vec3& obbPoint : _obb.m_Coordinates)
    {
        const f32 distanceOnAxis = glm::dot(obbPoint, axis);

        obbMax = glm::max(obbMax, distanceOnAxis);
        obbMin = glm::min(obbMin, distanceOnAxis);
    }

    const f32 triDistance = glm::dot(_triangle.m_Vertices[0], axis);

    if (obbMax > triDistance && obbMin < triDistance)
    {
        const f32 penetration = triDistance - obbMin;

        assert(penetration >= 0.f);
        if (penetration < _inOutPenetration)
        {
            _inOutPenetration = penetration;
            _inOutAxis = -axis;
            _outDebugInfo = "Triangle Normal";
        }

        return true;
    }

    return false;
}

std::optional<CollisionResult> CollisionAlgorithms::CollideOBB_Triangle(const glm::mat4& _transform,
                                                                        const AABB& _bounds,
                                                                        const Triangle& _triangle,
                                                                        const glm::vec3& _triangleOffset)
{
    // TODO Pass in.
    const CollisionHelpers::OBBProperties obb = CollisionHelpers::GetOBBProperties(_transform, _bounds, _triangleOffset);

    if (_triangle.m_Vertices[1] ==  _triangle.m_Vertices[0] || _triangle.m_Vertices[2] ==  _triangle.m_Vertices[1] || _triangle.m_Vertices[2] ==  _triangle.m_Vertices[0])
    {
        return std::nullopt;
    }

    const glm::vec3 triEdge0 = glm::normalize(_triangle.m_Vertices[1] - _triangle.m_Vertices[0]);
    const glm::vec3 triEdge1 = glm::normalize(_triangle.m_Vertices[2] - _triangle.m_Vertices[1]);
    const glm::vec3 triEdge2 = glm::normalize(_triangle.m_Vertices[0] - _triangle.m_Vertices[2]);

    glm::vec3 chosenAxis = glm::vec3();
    f32 penetration = std::numeric_limits<f32>::max();
    std::string debug;

    if (
            !TestCrossProductAxis(obb.m_X, triEdge0, obb, _triangle, chosenAxis, penetration, debug)
         || !TestCrossProductAxis(obb.m_X, triEdge1, obb, _triangle, chosenAxis, penetration, debug)
         || !TestCrossProductAxis(obb.m_X, triEdge2, obb, _triangle, chosenAxis, penetration, debug)
         || !TestCrossProductAxis(obb.m_Y, triEdge0, obb, _triangle, chosenAxis, penetration, debug)
         || !TestCrossProductAxis(obb.m_Y, triEdge1, obb, _triangle, chosenAxis, penetration, debug)
         || !TestCrossProductAxis(obb.m_Y, triEdge2, obb, _triangle, chosenAxis, penetration, debug)
         || !TestCrossProductAxis(obb.m_Z, triEdge0, obb, _triangle, chosenAxis, penetration, debug)
         || !TestCrossProductAxis(obb.m_Z, triEdge1, obb, _triangle, chosenAxis, penetration, debug)
         || !TestCrossProductAxis(obb.m_Z, triEdge2, obb, _triangle, chosenAxis, penetration, debug))
    {
        return std::nullopt;
    }

    if (
            !TestAxis(obb.m_X, obb, _triangle, chosenAxis, penetration, debug)
         || !TestAxis(obb.m_Y, obb, _triangle, chosenAxis, penetration, debug)
         || !TestAxis(obb.m_Z, obb, _triangle, chosenAxis, penetration, debug))
    {
        return std::nullopt;
    }

    if (!TestTriangleNormalAxis(obb, _triangle, chosenAxis, penetration, debug))
    {
        return std::nullopt;
    }

    CollisionResult result;
    result.m_Normal = chosenAxis;
    result.m_Distance = -penetration;
    result.m_DebugInfo = debug;
    return result;
}
