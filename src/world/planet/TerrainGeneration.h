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

#include <src/graphics/Color.h>
#include <src/tools/globals.h>
#include <src/world/planet/Planet.h>
#include <src/world/terrain/TerrainSubstance.h>

struct WorldPosition;

enum class TerrainLevelOfDetail
{
    ActiveZone,
    NearVista,
    FarVista,
    Planetary
};

class TerrainGeneration
{
    friend class UIPlanetDebug;

public:
    using WeightedBiomeArray = std::vector<std::pair<f32, const Planet::Biome*>>;

    static f32 GetDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod = TerrainLevelOfDetail::ActiveZone);
    static Color GetColor(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod = TerrainLevelOfDetail::ActiveZone);
    static TerrainSubstance GetSubstance(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod = TerrainLevelOfDetail::ActiveZone);

    static const Planet::Biome& GetBiome(const Planet& _planet, const WorldPosition& _position);
    static void GetClosestBiomes(const Planet& _planet, const WorldPosition& _position, WeightedBiomeArray & _outBiomes);

    static f32 ClampDensity(f32 _density);

private:
    static f32 GetUnclampedDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod = TerrainLevelOfDetail::ActiveZone);
    static f32 ComputeBaseShapeDensity(const Planet& _planet, const WorldPosition& _position);
    static f32 ComputeBiomeDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod);

    static Planet::BlendableBiomeInputs BlendBiomeInputs(const WeightedBiomeArray& _biomes);

public:
    static constexpr f32 MINIMUM_VALID_DENSITY = -1.f;
    static constexpr f32 MAXIMUM_VALID_DENSITY = 1.f;
};
