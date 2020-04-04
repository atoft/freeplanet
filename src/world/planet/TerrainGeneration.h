//
// Created by alastair on 29/03/2020.
//

#pragma once

#include <src/graphics/Color.h>
#include <src/tools/globals.h>
#include <src/world/planet/Planet.h>

struct WorldPosition;

enum class TerrainLevelOfDetail
{
    ActiveZone,
    NearVista,
    Planetary
};

class TerrainGeneration
{
public:
    static f32 GetDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod = TerrainLevelOfDetail::ActiveZone);
    static Color GetColor(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod = TerrainLevelOfDetail::ActiveZone);

    static const Planet::Biome& GetBiome(const Planet& _planet, const WorldPosition& _position);

private:
    static f32 ComputeBaseShapeDensity(const Planet& _planet, const WorldPosition& _position);
};
