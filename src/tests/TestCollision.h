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
    bool CollideNonOverlappingOBBTriangleEdges();

    bool CollideOverlappingOBBTriangleNormal();
    bool CollideNonOverlappingOBBTriangleNormal();

    static constexpr f32 TEST_COLLISION_EPS = glm::epsilon<f32>() * 2.f;
}
