/*
 * Copyright 2017-2020 Alastair Toft
 *
 * This file is part of freeplanet.
 *
 * freeplanet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeplanet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeplanet. If not, see <http://www.gnu.org/licenses/>.
 */

#include "RaycastAlgorithms.h"

#include <src/world/collision/CollisionHelpers.h>

std::optional<RaycastResult> RaycastAlgorithms::RaycastTerrain(
        const glm::vec3& _localOrigin,
        const glm::vec3& _direction,
        const std::vector<TerrainChunk>& _terrain)
{
    f32 minDistance = std::numeric_limits<f32>::max();
    bool foundCollision = false;

    for (const TerrainChunk& chunk : _terrain)
    {
        for (u32 triIndex = 0; triIndex < chunk.m_Count; ++triIndex)
        {
            const Triangle& tri = chunk.m_Triangles[triIndex];

            // Get two vectors in the triangle in order to find the triangle normal
            const glm::vec3 triangleSide1 = tri.m_Vertices[1] - tri.m_Vertices[0];
            const glm::vec3 triangleSide2 = tri.m_Vertices[2] - tri.m_Vertices[1];
            const glm::vec3 triangleSide3 = tri.m_Vertices[0] - tri.m_Vertices[2];

            if(triangleSide1 == glm::vec3() || triangleSide2 == glm::vec3() || triangleSide3 == glm::vec3())
            {
                // This is an invalid triangle, so we skip it.
                // These seem to occur occasionally from the marching cubes algorithm. :(
                continue;
            }

            const glm::vec3 axisA = glm::normalize(triangleSide1);
            const glm::vec3 axisB = glm::normalize(tri.m_Vertices[2] - tri.m_Vertices[0]);

            if (glm::abs(glm::dot(axisA, axisB)) > 0.99f)
            {
                // The axes are close to parallel, so skip this test.
                continue;
            }

            const glm::vec3 triangleNormal = glm::normalize(glm::cross(axisA, axisB));

            const f32 planeDistanceFromOrigin = -glm::dot(triangleNormal, tri.m_Vertices[0]);
            const f32 normalDotRayDirection = glm::dot(triangleNormal, _direction);

            if(glm::abs(normalDotRayDirection) <= glm::epsilon<float>())
            {
                // Ray and triangle parallel
                continue;
            }

            const f32 rayDistanceToPlane = -(glm::dot(triangleNormal, _localOrigin) + planeDistanceFromOrigin) / normalDotRayDirection;

            if(rayDistanceToPlane <= 0.f)
            {
                // Triangle behind ray
                continue;
            }

            const glm::vec3 point = _localOrigin + _direction * rayDistanceToPlane;

            // P = O + tX
            // N.P = d

            if(rayDistanceToPlane >= minDistance)
            {
                // Already found a closer collision
                continue;
            }

            // Vertices to point
            const glm::vec3 p0 = point - tri.m_Vertices[0];
            const glm::vec3 p1 = point - tri.m_Vertices[1];
            const glm::vec3 p2 = point - tri.m_Vertices[2];

            if(    glm::dot(triangleNormal, glm::cross(triangleSide1, p0)) >= 0.f
                && glm::dot(triangleNormal, glm::cross(triangleSide2, p1)) >= 0.f
                && glm::dot(triangleNormal, glm::cross(triangleSide3, p2)) >= 0.f)
            {
                // Point inside triangle!
                minDistance = rayDistanceToPlane;
                foundCollision = true;
            }
        }
    }

    if (foundCollision)
    {
        RaycastResult result;
        result.m_Distance = minDistance;
        result.m_HitObjectID = WORLDOBJECTID_INVALID;

        return  result;
    }

    return std::nullopt;
}


