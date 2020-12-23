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

#include "TestCollision.h"

#include <src/tests/TestHelpers.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/GeometryTypes.h>
#include <src/world/collision/algorithms/CollideOBBOBB.h>
#include <src/world/collision/algorithms/CollideOBBTriangle.h>

bool Test::TestCollision()
{
    bool result = true;

    // AABBs
    result &= CollideOverlappingAABBPair();
    result &= CollideNonOverlappingAABBPair();

    // OBBs
    result &= CollideOverlappingOBBPairFaces();
    result &= CollideNonOverlappingOBBPairFaces();

    result &= CollideOverlappingOBBPairFaceWithEdge();
    result &= CollideNonOverlappingOBBPairFaceWithEdge();

    result &= CollideOverlappingOBBPairEdgeWithEdge();
    result &= CollideNonOverlappingOBBPairEdgeWithEdge();

    // OBB - triangle
    result &= CollideOverlappingOBBTriangleFaces();
    result &= CollideOverlappingOBBTriangleFacesWithOffset();
    result &= CollideNonOverlappingOBBTriangleFaces();

    result &= CollideOverlappingOBBTriangleEdges();
    result &= CollideNonOverlappingOBBTriangleEdges();

    result &= CollideOverlappingOBBTriangleNormal();
    result &= CollideNonOverlappingOBBTriangleNormal();

    return result;
}

bool Test::CollideOverlappingAABBPair()
{
    const AABB box1 = AABB(glm::vec3(1.f));
    const AABB box2 = AABB(glm::vec3(2.f));

    const glm::mat4 transform1 = glm::translate(glm::mat4(1.f), glm::vec3(-.5f));
    const glm::mat4 transform2 = glm::translate(glm::mat4(1.f), glm::vec3(.5f));

    const std::optional<CollisionResult> collisionResult = CollideOBBOBB::Collide(transform1, box1, transform2, box2);

    return TestResult(collisionResult.has_value() && MathsHelpers::EqualWithEpsilon(collisionResult->m_Normal, glm::vec3(1.f,0.f,0.f), TEST_COLLISION_EPS));
}

bool Test::CollideNonOverlappingAABBPair()
{
    const AABB box1 = AABB(glm::vec3(1.f));
    const AABB box2 = AABB(glm::vec3(2.f));

    const glm::mat4 transform1 = glm::translate(glm::mat4(1.f), glm::vec3(-.5f, -.5f, -2.6f));
    const glm::mat4 transform2 = glm::translate(glm::mat4(1.f), glm::vec3(.5f));

    const std::optional<CollisionResult> collisionResult = CollideOBBOBB::Collide(transform1, box1, transform2, box2);

    return TestResult(collisionResult == std::nullopt);
}

bool Test::CollideOverlappingOBBPairFaces()
{
    const AABB box1 = AABB(glm::vec3(1.f));
    const AABB box2 = AABB(glm::vec3(1.f, 2.f, 1.f));

    const glm::mat4 transform1 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -2.5f, 0.f));
    const glm::mat4 transform2 = glm::translate(glm::mat4(1.f), glm::vec3(.5f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 8.f, glm::vec3(0,1,0));

    const std::optional<CollisionResult> collisionResult = CollideOBBOBB::Collide(transform1, box1, transform2, box2);

    return TestResult(collisionResult.has_value() && MathsHelpers::EqualWithEpsilon(collisionResult->m_Normal, glm::vec3(0.f,1.f,0.f), TEST_COLLISION_EPS));
}

bool Test::CollideNonOverlappingOBBPairFaces()
{
    const AABB box1 = AABB(glm::vec3(1.f));
    const AABB box2 = AABB(glm::vec3(1.f, 2.f, 1.f));

    const glm::mat4 transform1 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -2.51f, 0.f));
    const glm::mat4 transform2 = glm::translate(glm::mat4(1.f), glm::vec3(.5f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 8.f, glm::vec3(0,1,0));

    const std::optional<CollisionResult> collisionResult = CollideOBBOBB::Collide(transform1, box1, transform2, box2);

    return TestResult(collisionResult == std::nullopt);
}

bool Test::CollideOverlappingOBBPairFaceWithEdge()
{
    const AABB box1 = AABB(glm::vec3(1.f));
    const AABB box2 = AABB(glm::vec3(1.f, 1.f, 1.f));

    const glm::mat4 transform1 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
    const glm::mat4 transform2 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 2.41f, 0.f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 4.f, glm::vec3(0,0,1));

    const std::optional<CollisionResult> collisionResult = CollideOBBOBB::Collide(transform1, box1, transform2, box2);

    return TestResult(collisionResult.has_value() && MathsHelpers::EqualWithEpsilon(collisionResult->m_Normal, glm::vec3(0.f,1.f,0.f), TEST_COLLISION_EPS));
}

bool Test::CollideNonOverlappingOBBPairFaceWithEdge()
{
    const AABB box1 = AABB(glm::vec3(1.f));
    const AABB box2 = AABB(glm::vec3(1.f));

    const glm::mat4 transform1 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
    const glm::mat4 transform2 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 2.42f, 0.f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 4.f, glm::vec3(0,0,1));

    const std::optional<CollisionResult> collisionResult = CollideOBBOBB::Collide(transform1, box1, transform2, box2);

    return TestResult(collisionResult == std::nullopt);
}

bool Test::CollideOverlappingOBBPairEdgeWithEdge()
{
    const AABB box1 = AABB(glm::vec3(1.f));
    const AABB box2 = AABB(glm::vec3(1.f));

    const glm::mat4 transform1 = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, -2.f, 0.f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 4.f, glm::vec3(1,0,0));
    const glm::mat4 transform2 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.82f, 0.f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 4.f, glm::vec3(0,0,1));

    const std::optional<CollisionResult> collisionResult = CollideOBBOBB::Collide(transform1, box1, transform2, box2);

    return TestResult(collisionResult.has_value() && MathsHelpers::EqualWithEpsilon(collisionResult->m_Normal, glm::vec3(0.f,1.f,0.f), TEST_COLLISION_EPS));
}

bool Test::CollideNonOverlappingOBBPairEdgeWithEdge()
{
    const AABB box1 = AABB(glm::vec3(1.f));
    const AABB box2 = AABB(glm::vec3(1.f));

    const glm::mat4 transform1 = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, -2.f, 0.f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 4.f, glm::vec3(1,0,0));
    const glm::mat4 transform2 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.84f, 0.f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 4.f, glm::vec3(0,0,1));

    const std::optional<CollisionResult> collisionResult = CollideOBBOBB::Collide(transform1, box1, transform2, box2);

    return TestResult(collisionResult == std::nullopt);
}

bool Test::CollideOverlappingOBBTriangleFaces()
{
    const AABB box = AABB(glm::vec3(1.f, 2.f, 1.f));
    const glm::mat4 boxTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));

    const Triangle triangle {{glm::vec3(-2.f, -1.5f, -2.f), glm::vec3(2.f, -1.5f, -2.f), glm::vec3(0.f, -1.2f, 2.f)}};

    const std::optional<CollisionResult> collisionResult = CollideOBBTriangle::Collide(boxTransform, box, triangle, glm::vec3());
    return TestResult(collisionResult.has_value() && MathsHelpers::EqualWithEpsilon(collisionResult->m_Normal, glm::vec3(0.f,-1.f,0.f), TEST_COLLISION_EPS));
}

bool Test::CollideOverlappingOBBTriangleFacesWithOffset()
{
    const AABB box = AABB(glm::vec3(1.f, 2.f, 1.f));
    const glm::mat4 boxTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));

    const Triangle triangle {{glm::vec3(-2.f, -6.5f, -2.f), glm::vec3(2.f, -6.5f, -2.f), glm::vec3(0.f, -6.2f, 2.f)}};

    const std::optional<CollisionResult> collisionResult = CollideOBBTriangle::Collide(boxTransform, box, triangle, glm::vec3(0.f, 5.f, 0.f));

    return TestResult(collisionResult.has_value() && MathsHelpers::EqualWithEpsilon(collisionResult->m_Normal, glm::vec3(0.f,-1.f,0.f), TEST_COLLISION_EPS));
}

bool Test::CollideNonOverlappingOBBTriangleFaces()
{
    const AABB box = AABB(glm::vec3(1.f, 2.f, 1.f));
    const glm::mat4 boxTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 0.f));

    const Triangle triangle {{glm::vec3(-2.f, -1.5f, -2.f), glm::vec3(2.f, -1.5f, -2.f), glm::vec3(0.f, -1.2f, 2.f)}};

    const std::optional<CollisionResult> collisionResult = CollideOBBTriangle::Collide(boxTransform, box, triangle, glm::vec3());

    return TestResult(collisionResult == std::nullopt);
}

bool Test::CollideOverlappingOBBTriangleEdges()
{
    const AABB box = AABB(glm::vec3(2.f, 2.f, 1.f));
    const glm::mat4 boxTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 4.f, glm::vec3(0,0,1));

    const Triangle triangle {{glm::vec3(-2.f, 2.8f, 0.f), glm::vec3(2.f, 2.8f, 0.f), glm::vec3(0.f, 4.f, 2.f)}};

    const std::optional<CollisionResult> collisionResult = CollideOBBTriangle::Collide(boxTransform, box, triangle, glm::vec3());
    return TestResult(collisionResult.has_value());
}

bool Test::CollideNonOverlappingOBBTriangleEdges()
{
    const AABB box = AABB(glm::vec3(2.f, 2.f, 1.f));
    const glm::mat4 boxTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f)) * glm::rotate(glm::mat4(1.f), glm::pi<f32>() / 4.f, glm::vec3(0,0,1));

    const Triangle triangle {{glm::vec3(-2.f, 2.9f, 0.f), glm::vec3(2.f, 2.9f, 0.f), glm::vec3(0.f, 4.f, 2.f)}};

    const std::optional<CollisionResult> collisionResult = CollideOBBTriangle::Collide(boxTransform, box, triangle, glm::vec3());
    return TestResult(collisionResult == std::nullopt);
}

bool Test::CollideOverlappingOBBTriangleNormal()
{
    const AABB box = AABB(glm::vec3(2.f, 2.f, 2.f));
    const glm::mat4 boxTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));

    const Triangle triangle {{glm::vec3(-2.1f, -1.5f, -2.1f), glm::vec3(-1.5f, -2.1f, -2.1f), glm::vec3(-2.1f, -2.1f, -1.5f)}};

    const std::optional<CollisionResult> collisionResult = CollideOBBTriangle::Collide(boxTransform, box, triangle, glm::vec3());
    return TestResult(collisionResult.has_value());
}

bool Test::CollideNonOverlappingOBBTriangleNormal()
{
    const AABB box = AABB(glm::vec3(2.f, 2.f, 2.f));
    const glm::mat4 boxTransform = glm::translate(glm::mat4(1.f), glm::vec3(0.1f));

    const Triangle triangle {{glm::vec3(-2.1f, -1.5f, -2.1f), glm::vec3(-1.5f, -2.1f, -2.1f), glm::vec3(-2.1f, -2.1f, -1.5f)}};

    const std::optional<CollisionResult> collisionResult = CollideOBBTriangle::Collide(boxTransform, box, triangle, glm::vec3());
    return TestResult(collisionResult == std::nullopt);
}
