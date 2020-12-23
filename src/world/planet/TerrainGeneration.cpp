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

#include "TerrainGeneration.h"

#include <algorithm>
#include <glm/gtc/noise.hpp>

#include <src/world/planet/Planet.h>
#include <src/world/WorldPosition.h>
#include <src/tools/MathsHelpers.h>
#include <random>

f32 TerrainGeneration::GetDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    return ClampDensity(GetUnclampedDensity(_planet, _position, _lod));
}

f32 TerrainGeneration::GetUnclampedDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    f32 density = 0.f;

    density += ComputeBaseShapeDensity(_planet, _position);

    // Outside this range, density cannot be affected by the biome.
    // Avoid the biome noise computations in that case to gain a lot of performance.
    if (density >= 2.f * MINIMUM_VALID_DENSITY && density <= 2.f * MAXIMUM_VALID_DENSITY)
    {
        density += ComputeBiomeDensity(_planet, _position, _lod);
    }

    return density;
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

TerrainSubstance TerrainGeneration::GetSubstance(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    // @Performance Should reuse the same density sample as for the mesh generation.
    const float density = GetUnclampedDensity(_planet, _position, _lod);

    constexpr float DIRT_MIN_DENSITY = 0.1f;
    constexpr float ROCK_MIN_DENSITY = 0.2f;

    TerrainSubstance result;
    result.m_Topsoil = glm::clamp((DIRT_MIN_DENSITY - density) / DIRT_MIN_DENSITY, 0.f, 1.f);

    result.m_Rock = glm::clamp((density - ROCK_MIN_DENSITY) / ROCK_MIN_DENSITY, 0.f, 1.f);

    result.m_Dirt = (1.f - result.m_Topsoil) * (1.f - result.m_Rock);
    return result;
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

// Keep all densities in the terrain pipeline in this range, so that terrain edits behave predictably.
f32 TerrainGeneration::ClampDensity(f32 _density)
{
    return glm::clamp(_density, MINIMUM_VALID_DENSITY, MAXIMUM_VALID_DENSITY);
}

f32 TerrainGeneration::ComputeBaseShapeDensity(const Planet& _planet, const WorldPosition& _position)
{
    constexpr f32 scaleFactor = 16.f;

    switch (_planet.m_Shape)
    {
    case Planet::BaseShape::Sphere:
    {
         const f32 density = (_planet.m_Radius - glm::length(_position.GetPositionRelativeTo(_planet.m_OriginZone))) / scaleFactor;

         // Don't clamp here as we want to strictly enforce that there is less density higher up when generating
         // terrain. A later clamp with remove very large values before the final result is returned.
         return density;
    }
    case Planet::BaseShape::Torus:
    {
        constexpr f32 torusThickness = 256.f;

        const glm::vec3 globalPosition = _position.GetPositionRelativeTo(_planet.m_OriginZone);

        const glm::vec2 torusPosition = glm::vec2(globalPosition.x, globalPosition.y);

        const glm::vec2 circlePosition = glm::vec2(glm::length(torusPosition) - _planet.m_Radius, globalPosition.z);
        return (torusThickness - glm::length(circlePosition)) / scaleFactor;
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
    case TerrainLevelOfDetail::FarVista:
        return 3;
    case TerrainLevelOfDetail::Planetary:
        [[fallthrough]];
    default:
        return 1;
    }
}

f32 TerrainGeneration::ComputeBiomeDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    f32 density = 0.f;

    WeightedBiomeArray biomes;
    GetClosestBiomes(_planet, _position, biomes);
    assert(biomes.size() == 3);

    const Planet::BlendableBiomeInputs inputs = BlendBiomeInputs(biomes);

    const u32 maxOctaves = GetMaxOctavesForLOD(_lod);

    const f32 baseFrequency = 0.025f;
    const f32 baseAmplitude = 0.5f;

    constexpr f32 freqMultiplier = 2.f;
    constexpr f32 amplitudeMultiplier = 0.5f;

    // TODO How can we avoid float precision issues with noise?
    const glm::vec3 planetaryPosition = _position.GetPositionRelativeTo(_planet.m_OriginZone);

    f32 frequency = baseFrequency;
    f32 amplitude = baseAmplitude;

    for (u32 octave = 1; octave <= maxOctaves; ++octave)
    {
        density += ClampDensity(glm::perlin(planetaryPosition * frequency + _planet.m_OctaveOffsets[octave - 1]) * amplitude * inputs.m_OctaveWeights[octave - 1]);

        frequency *= freqMultiplier;
        amplitude *= amplitudeMultiplier;
    }

    return ClampDensity(density);
}

Planet::BlendableBiomeInputs TerrainGeneration::BlendBiomeInputs(const WeightedBiomeArray& _biomes)
{
    if (_biomes[0].first == 1.f)
    {
        return _biomes[0].second->m_Inputs;
    }

    Planet::BlendableBiomeInputs resultingInputs;

    for (f32& resultingWeight : resultingInputs.m_OctaveWeights)
    {
        resultingWeight = 0.f;
    }

    for (const auto& biome : _biomes)
    {
        u32 octaveIdx = 0;
        for (f32& resultingWeight : resultingInputs.m_OctaveWeights)
        {
            resultingWeight += biome.second->m_Inputs.m_OctaveWeights[octaveIdx] * biome.first;

            ++octaveIdx;
        }
    }

    return resultingInputs;
}
