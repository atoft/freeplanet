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
