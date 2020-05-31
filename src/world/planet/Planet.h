//
// Created by alastair on 28/03/2020.
//

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
