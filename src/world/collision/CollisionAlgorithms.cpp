//
// Created by alastair on 11/02/19.
//

#include "CollisionAlgorithms.h"

#include <vector>
#include <optional>

#include <src/tools/MathsHelpers.h>
#include <src/world/collision/CollisionHelpers.h>

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
                                             const std::vector<TerrainChunk>& _terrain,
                                             const glm::vec3& _terrainOffset)
{
    const CollisionHelpers::OBBProperties properties = CollisionHelpers::GetOBBProperties(_transform1, _bounds1, _terrainOffset);

    const glm::vec3& obbX = properties.m_X;
    const glm::vec3& obbY = properties.m_Y;
    const glm::vec3& obbZ = properties.m_Z;
    const std::array<glm::vec3, 8>& obbCoordinates = properties.m_Coordinates;

    glm::vec3 overallChosenAxis = glm::vec3();
    f32 overallSeparation = -std::numeric_limits<f32>::max();
    bool didCollide = false;

    for (const TerrainChunk& chunk : _terrain)
    {
        for (u32 triIdx = 0; triIdx < chunk.m_Count; ++triIdx)
        {
            glm::vec3 possibleAxis = glm::vec3();
            f32 possibleSeparation = -std::numeric_limits<f32>::max();

            const Triangle& tri = chunk.m_Triangles[triIdx];

            // Collision detection in axes of OBB  x3
            if (   !CollisionHelpers::IsTriangleCollisionInAxis(obbCoordinates, tri, obbX, possibleSeparation, possibleAxis)
                || !CollisionHelpers::IsTriangleCollisionInAxis(obbCoordinates, tri, obbY, possibleSeparation, possibleAxis)
                || !CollisionHelpers::IsTriangleCollisionInAxis(obbCoordinates, tri, obbZ, possibleSeparation, possibleAxis))
            {
                continue;
            }

            // Get two vectors in the triangle in order to find the triangle normal
            glm::vec3 triangleSide1 = tri.m_Vertices[2] - tri.m_Vertices[0];
            glm::vec3 triangleSide2 = tri.m_Vertices[1] - tri.m_Vertices[0];

            if (triangleSide1 == glm::vec3() || triangleSide2 == glm::vec3())
            {
                // This is an invalid triangle, so we skip it.
                // These seem to occur occasionally from the marching cubes algorithm. :(
                continue;
            }

            const std::optional<glm::vec3> triangleNormal = CollisionHelpers::TryNormalizedCross(
                    glm::normalize(triangleSide1),
                    glm::normalize(triangleSide2));

            if (triangleNormal.has_value())
            {
                // Collision detection in axes of cross products of edges of OBB and triangle normal  x3
                if (   !CollisionHelpers::IsTriangleCollisionInAxis(obbCoordinates, tri, CollisionHelpers::TryNormalizedCross(obbX, *triangleNormal), possibleSeparation, possibleAxis)
                    || !CollisionHelpers::IsTriangleCollisionInAxis(obbCoordinates, tri, CollisionHelpers::TryNormalizedCross(obbY, *triangleNormal), possibleSeparation, possibleAxis)
                    || !CollisionHelpers::IsTriangleCollisionInAxis(obbCoordinates, tri, CollisionHelpers::TryNormalizedCross(obbZ, *triangleNormal), possibleSeparation, possibleAxis))
                {
                    continue;
                }

                // In the normal of the tri, check tri is inside but not in front of the obb.
                // If it is, this gives us the intersection distance.

                f32 obbMinDistanceOnTriNormal = std::numeric_limits<f32>::max();
                f32 obbMaxDistanceOnTriNormal = -std::numeric_limits<f32>::max();

                const f32 triDistanceOnNormal = glm::dot(tri.m_Vertices[0], *triangleNormal);

                for (u32 i = 0; i < 8; ++i)
                {
                    const f32 distOnAxis1 = glm::dot(obbCoordinates[i], *triangleNormal);

                    // If further along the axis in + or - dir, set as our max or min
                    obbMaxDistanceOnTriNormal = glm::max(obbMaxDistanceOnTriNormal, distOnAxis1);
                    obbMinDistanceOnTriNormal = glm::min(obbMinDistanceOnTriNormal, distOnAxis1);
                }

                const f32 overlap = obbMaxDistanceOnTriNormal - triDistanceOnNormal;
                const f32 span = obbMaxDistanceOnTriNormal - obbMinDistanceOnTriNormal;

                if ( overlap > span /* obb behind triangle */
                    || overlap < 0 /* obb in front of triangle*/ )
                {
                    continue;
                }


                // I think the jumping issue is related to a case where the box is intersecting the triangle,
                // but is not actually overlapping the full distance in the triangle normal direction.
                // That can be the case when the box is clipping an edge of the triangle.
                // I think, in that case, we should use the axis of the box with the smallest overlap distance.


                didCollide = true;

                if (overlap > possibleSeparation)
                {
                    possibleSeparation = -overlap;
                    possibleAxis = *triangleNormal;
                }

                if (possibleSeparation > overallSeparation)
                {
                    overallSeparation = possibleSeparation;
                    overallChosenAxis = possibleAxis;
                }

                // HACK
                break;
            }
        }

        // HACK
        if (didCollide)
        {
            break;
        }
    }

    if(didCollide && overallSeparation <= 0.f)
    {
        CollisionResult result;
        assert(MathsHelpers::IsNormalized(overallChosenAxis));
        result.m_Normal = overallChosenAxis;
        result.m_Distance = overallSeparation;
        return result;
    }
    else
    {
        return std::nullopt;
    }
}
