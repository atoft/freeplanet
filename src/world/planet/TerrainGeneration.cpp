//
// Created by alastair on 29/03/2020.
//

#include "TerrainGeneration.h"

#include <src/world/planet/Planet.h>
#include <src/world/WorldPosition.h>
#include <src/tools/MathsHelpers.h>

f32 TerrainGeneration::GetDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    f32 density = 0.f;

    density += ComputeBaseShapeDensity(_planet, _position);

    (void)(_lod);

    return density;
}

// Later, will probably want to expose materials with blending rather than color, e.g. 0.9 stone, 0.1 dirt
// And let the shader draw them as needed.
Color TerrainGeneration::GetColor(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    const Planet::Biome biome = GetBiome(_planet, _position);

    return biome.m_GroundColor;
}

const Planet::Biome& TerrainGeneration::GetBiome(const Planet& _planet, const WorldPosition& _position)
{
    assert(!_planet.m_Biomes.empty());

    if (_planet.m_Biomes.size() > 1)
    {
        const glm::vec3 planetaryPosition = _position.GetPositionRelativeTo(_planet.m_OriginZone);

        if (MathsHelpers::EqualWithEpsilon(planetaryPosition, glm::vec3(0.f), glm::epsilon<f32>()))
        {
            return _planet.m_Biomes[0];
        }

        const glm::vec3 targetDirection = glm::normalize(planetaryPosition);

        const Planet::Biome* bestBiome = nullptr;
        f32 bestDotProduct = -std::numeric_limits<f32>::max();

        for (const Planet::Biome& biome : _planet.m_Biomes)
        {
            const f32 dotProduct = glm::dot(biome.m_BiomeDirection, targetDirection);

            if (dotProduct > bestDotProduct)
            {
                bestDotProduct = dotProduct;
                bestBiome = &biome;
            }
        }

        assert(bestBiome != nullptr);
        return *bestBiome;
    }

    return _planet.m_Biomes[0];
}

f32 TerrainGeneration::ComputeBaseShapeDensity(const Planet& _planet, const WorldPosition& _position)
{
    switch (_planet.m_Shape)
    {
    case Planet::BaseShape::Sphere:
    {
         const f32 density = _planet.m_Radius - glm::length(_position.GetPositionRelativeTo(_planet.m_OriginZone));

         // TODO any min/max clamping here.

         return density;
    }
    default:
        return 0.f;
    }
}
