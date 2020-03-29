//
// Created by alastair on 29/03/2020.
//

#include "TerrainGeneration.h"

#include <src/world/planet/Planet.h>
#include <src/world/WorldPosition.h>

f32 TerrainGeneration::GetDensity(const Planet& _planet, const WorldPosition& _position, TerrainLevelOfDetail _lod)
{
    f32 density = 0.f;

    density += ComputeBaseShapeDensity(_planet, _position);

    (void)(_lod);

    return density;
}

f32 TerrainGeneration::ComputeBaseShapeDensity(const Planet& _planet, const WorldPosition& _position)
{
    switch (_planet.m_Shape)
    {
    case Planet::BaseShape::Sphere:
    {
        const glm::ivec3 planetCoreZone = glm::ivec3(0);

         const f32 density = _planet.m_Radius - glm::length(_position.GetPositionRelativeTo(planetCoreZone));

         // TODO any min/max clamping here.

         return density;
    }
    default:
        return 0.f;
    }
}
