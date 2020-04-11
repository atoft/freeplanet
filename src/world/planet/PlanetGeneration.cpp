//
// Created by alastair on 10/04/2020.
//

#include "PlanetGeneration.h"

#include <random>

#include <src/tools/MathsHelpers.h>
#include <src/world/WorldPosition.h>

// TODO At some point we will need extra context passed in, if we are generating a solar system
// with several planets.
Planet PlanetGeneration::GenerateFromSeed(u32 _seed)
{
    Planet planet;
    planet.m_TerrainSeed = _seed;
    planet.m_Shape = Planet::BaseShape::Sphere;

    constexpr u32 biomeCount = 100;

    std::mt19937 gen(planet.m_TerrainSeed);
    std::uniform_real_distribution<> unsignedDistribution(0.f, 1.f);
    std::uniform_real_distribution<> signedDistribution(-1.f, 1.f);

    std::vector<glm::vec2> pitchYaws;
    GenerateFibonacciSphere(biomeCount, pitchYaws);

    for (u32 biomeIdx = 0; biomeIdx < biomeCount; ++biomeIdx)
    {
        Planet::Biome biome;
        biome.m_GroundColor = Color(unsignedDistribution(gen), unsignedDistribution(gen), unsignedDistribution(gen), 1);
        biome.m_PitchRadians = pitchYaws[biomeIdx].x;
        biome.m_YawRadians = pitchYaws[biomeIdx].y;
        biome.m_BiomeDirection = MathsHelpers::GenerateNormalFromPitchYaw(biome.m_PitchRadians,
                                                                          biome.m_YawRadians);

        for (f32& weight : biome.m_Inputs.m_OctaveWeights)
        {
            weight = 1.375f + signedDistribution(gen) * 0.625f;
        }

        planet.m_Biomes.push_back(biome);
    }

    constexpr f32 noiseOffsetRange = 100.f;
    for (glm::vec3& offset : planet.m_OctaveOffsets)
    {
        offset = glm::vec3(signedDistribution(gen), signedDistribution(gen), signedDistribution(gen)) * noiseOffsetRange;
    }

    return planet;
}

glm::vec3 PlanetGeneration::GetUpDirection(const Planet& _planet, const WorldPosition& _position)
{
    return glm::normalize(_position.GetPositionRelativeTo(_planet.m_OriginZone));
}

// Generates an even distribution of points across a sphere, based on http://extremelearning.com.au/evenly-distributing-points-on-a-sphere/.
void PlanetGeneration::GenerateFibonacciSphere(u32 _count, std::vector<glm::vec2>& _outPitchYaws)
{
    // The golden ratio.
    const f32 phi = (1.f + glm::sqrt(5.f)) / 2.f;

    _outPitchYaws.reserve(_count);

    // First point.
    _outPitchYaws.emplace_back(glm::half_pi<f32>(), 0.f);

    for (u32 index = 1; index < _count - 1; ++index)
    {
        const f32 x = (static_cast<f32>(index) + 6.f) / (static_cast<f32>(_count) + 11.f);
        const f32 y = index / phi;

        const f32 pitch = glm::acos(2.f * x - 1.f) - glm::half_pi<f32>();
        const f32 yaw = 2.f * glm::pi<f32>() * y;

        _outPitchYaws.emplace_back(pitch, yaw);
    }

    // Last point.
    _outPitchYaws.emplace_back(-glm::half_pi<f32>(), 2.f * glm::pi<f32>());
}
