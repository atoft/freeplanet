//
// Created by alastair on 23/08/19.
//

#pragma once

#include <glm/vec3.hpp>

#include <src/tools/globals.h>

class TerrainConstants
{
public:
    static constexpr f32 WORLD_ZONE_SIZE = 32;
    static constexpr u32 CHUNKS_PER_ZONE_EDGE = 32;

    // How far away should we load zones relative to a player's current zone?
    static const glm::ivec3 WORLD_ZONE_LOAD_DISTANCE;
};
