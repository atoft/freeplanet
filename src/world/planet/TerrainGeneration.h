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
    static void GetClosestBiomes(const Planet& _planet, const WorldPosition& _position, std::vector<std::pair<f32, const Planet::Biome*>>& _outBiomes);

    static void GenerateFibonacciSphere(u32 _count, std::vector<glm::vec2>& _outPitchYaws);

    static f32 ClampDensity(f32 _density);

private:

    static f32 ComputeBaseShapeDensity(const Planet& _planet, const WorldPosition& _position);
    static f32 ComputeBiomeDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod);
};
