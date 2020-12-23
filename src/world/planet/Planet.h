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

#include <vector>

#include <src/graphics/Color.h>
#include <src/tools/globals.h>

// Planet contains all the properties needed to generate any part of the world.
// The values themselves might be entirely generated from a seed, from some presets, or all set explicitly.
struct Planet
{
    enum class BaseShape
    {
        Sphere,
        Torus,
        Blobs
    };

    BaseShape m_Shape = BaseShape::Sphere;
    f32 m_Radius = 1024.f;
    f32 m_AtmosphereHeight = 96.f;
    f32 m_BlendOutHeight = 32.f;
    glm::ivec3 m_OriginZone = glm::ivec3(0);

    // How much extra space there is outside the planet's atmosphere in which WorldZones can exist.
    f32 m_WorldPadding = 64.f;

    u32 m_TerrainSeed = 0;

    glm::vec3 m_OctaveOffsets[4];

    struct BlendableBiomeInputs
    {
        f32 m_OctaveWeights[4] = {1.f, 1.f, 1.f, 1.f};
    };

    struct Biome
    {
        f32 m_PitchRadians = 0.f;
        f32 m_YawRadians = 0.f;
        glm::vec3 m_BiomeDirection = glm::vec3(1.f,0.f,0.f);

        Color m_GroundColor = Color(0.f,1.f,0.f,1.f);
        f32 m_TreeCoverage = 0.f;

        BlendableBiomeInputs m_Inputs;
    };

    std::vector<Biome> m_Biomes;
};
