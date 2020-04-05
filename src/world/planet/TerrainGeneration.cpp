//
// Created by alastair on 29/03/2020.
//

#include "TerrainGeneration.h"

#include <algorithm>
#include <glm/gtc/noise.hpp>

#include <src/world/planet/Planet.h>
#include <src/world/WorldPosition.h>
#include <src/tools/MathsHelpers.h>
#include <random>

f32 TerrainGeneration::GetDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    f32 density = 0.f;

    density += ComputeBaseShapeDensity(_planet, _position);
    density += ComputeBiomeDensity(_planet, _position, _lod);

    (void)(_lod);

    return ClampDensity(density);
}

// Later, will probably want to expose materials with blending rather than color, e.g. 0.9 stone, 0.1 dirt
// And let the shader draw them as needed.
Color TerrainGeneration::GetColor(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    std::vector<std::pair<f32, const Planet::Biome*>> biomes;
    GetClosestBiomes(_planet, _position, biomes);
    assert(biomes.size() == 3);

    Color color = Color(0.f, 0.f, 0.f, 1.f);
    color += biomes[0].first * biomes[0].second->m_GroundColor;
    color += biomes[1].first * biomes[1].second->m_GroundColor;
    color += biomes[2].first * biomes[2].second->m_GroundColor;
    color.a = 1.f;

    return color;
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

void TerrainGeneration::GetClosestBiomes(const Planet& _planet, const WorldPosition& _position, std::vector<std::pair<f32, const Planet::Biome*>>& _outBiomes)
{
    assert(!_planet.m_Biomes.empty());

    const glm::vec3 planetaryPosition = _position.GetPositionRelativeTo(_planet.m_OriginZone);

    if (MathsHelpers::EqualWithEpsilon(planetaryPosition, glm::vec3(0.f), glm::epsilon<f32>()))
    {
        return;
    }

    const glm::vec3 targetDirection = glm::normalize(planetaryPosition);

    for (const Planet::Biome& biome : _planet.m_Biomes)
    {
        const f32 dotProduct = glm::dot(biome.m_BiomeDirection, targetDirection);

        _outBiomes.emplace_back(dotProduct, &biome);
    }

    const u64 sizeToSort = std::min(static_cast<u64>(3), _outBiomes.size());

    std::partial_sort(
            _outBiomes.begin(),
            _outBiomes.begin() + sizeToSort + 1,
            _outBiomes.end(),
            [](const std::pair<f32, const Planet::Biome*>& _lhs, const std::pair<f32, const Planet::Biome*> _rhs)
            {
                return _lhs.first > _rhs.first;
            });

    _outBiomes.resize(sizeToSort);

    if (sizeToSort != 3)
    {
        return;
    }

    // Go from dot products to a blend weight.

    // TODO Allow a three-way blend.
    const f32 weightA = glm::clamp(_outBiomes[0].first, 0.f, 1.f);
    const f32 weightB = glm::clamp(_outBiomes[1].first, 0.f, 1.f);

    // Use a threshold so that we're only blending at the edges of the regions, not across the whole area.
    constexpr f32 minThreshold = 0.02f;

    const f32 blendA = 0.5f + glm::clamp((weightA - weightB) / minThreshold, -0.5f, 0.5f);
    const f32 blendB = 1.f - blendA;

    _outBiomes[0].first = blendA;
    _outBiomes[1].first = blendB;
    _outBiomes[2].first = 0.f;
}

// Generates and even distribution of points across a sphere, based on http://extremelearning.com.au/evenly-distributing-points-on-a-sphere/.
void TerrainGeneration::GenerateFibonacciSphere(u32 _count, std::vector<glm::vec2>& _outPitchYaws)
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

// Keep all densities in the terrain pipeline in this range, so that terrain edits behave predictably.
f32 TerrainGeneration::ClampDensity(f32 _density)
{
    constexpr f32 minDensity = -1.f;
    constexpr f32 maxDensity = 1.f;


    return glm::clamp(_density, minDensity, maxDensity);
}

f32 TerrainGeneration::ComputeBaseShapeDensity(const Planet& _planet, const WorldPosition& _position)
{
    switch (_planet.m_Shape)
    {
    case Planet::BaseShape::Sphere:
    {
         constexpr f32 scaleFactor = 16.f;
         const f32 density = (_planet.m_Radius - glm::length(_position.GetPositionRelativeTo(_planet.m_OriginZone))) / scaleFactor;

         // Don't clamp here as we want to strictly enforce that there is less density higher up when generating
         // terrain. A later clamp with remove very large values before the final result is returned.
         return density;
    }
    default:
        return 0.f;
    }
}

u32 GetMaxOctavesForLOD(TerrainLevelOfDetail _lod)
{
    switch (_lod)
    {
    case TerrainLevelOfDetail::ActiveZone:
        return 4;
    case TerrainLevelOfDetail::NearVista:
        return 3;
    case TerrainLevelOfDetail::Planetary:
        [[fallthrough]];
    default:
        return 0;
    }
}

f32 TerrainGeneration::ComputeBiomeDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    srand(_planet.m_TerrainSeed);

    f32 density = 0.f;

    // TODO get blended biome
    const Planet::Biome& biome = GetBiome(_planet, _position);

    const u32 maxOctaves = GetMaxOctavesForLOD(_lod);

    const f32 baseFrequency = 0.025f;
    const f32 baseAmplitude = 0.5f;

    constexpr f32 freqMultiplier = 2.f;
    constexpr f32 amplitudeMultiplier = 0.5f;

    // TODO How can we avoid float precision issues with noise?
    const glm::vec3 planetaryPosition = _position.GetPositionRelativeTo(_planet.m_OriginZone);

    f32 frequency = baseFrequency;
    f32 amplitude = baseAmplitude;

    for (u32 octave = 1; octave <= maxOctaves; ++ octave)
    {
        density += ClampDensity(glm::perlin(planetaryPosition * frequency) * amplitude * biome.m_OctaveWeights[octave - 1]);

        frequency *= freqMultiplier;
        amplitude *= amplitudeMultiplier;
    }

    return ClampDensity(density);
}