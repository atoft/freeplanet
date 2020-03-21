//
// Created by alastair on 14/03/2020.
//

#pragma once

#include <src/tools/globals.h>

namespace Test
{
    bool TestCollision();

    bool CollideOverlappingAABBPair();
    bool CollideNonOverlappingAABBPair();

    bool CollideOverlappingOBBPairFaces();
    bool CollideNonOverlappingOBBPairFaces();

    bool CollideOverlappingOBBPairFaceWithEdge();
    bool CollideNonOverlappingOBBPairFaceWithEdge();

    bool CollideOverlappingOBBPairEdgeWithEdge();
    bool CollideNonOverlappingOBBPairEdgeWithEdge();

    bool CollideOverlappingOBBTriangleFaces();
    bool CollideOverlappingOBBTriangleFacesWithOffset();
    bool CollideNonOverlappingOBBTriangleFaces();

    bool CollideOverlappingOBBTriangleEdges();

    static constexpr f32 TEST_COLLISION_EPS = glm::epsilon<f32>() * 2.f;
}
