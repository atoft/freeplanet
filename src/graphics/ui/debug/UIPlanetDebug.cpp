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

#include "UIPlanetDebug.h"

#include <src/world/planet/Planet.h>
#include <src/world/World.h>
#include <src/world/planet/TerrainGeneration.h>

void UIPlanetDebug::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    if (_world == nullptr)
    {
        return;
    }

    const Planet* planet = _world->GetPlanet();

    if (planet == nullptr)
    {
        _display.DrawString(glm::ivec2(20, 20), "The current world has no Planet.", 16.f);
        return;
    }

    std::string planetInfo;
    planetInfo += "Radius: " + std::to_string(planet->m_Radius) + "\n";
    planetInfo += std::to_string(planet->m_Biomes.size()) + " biomes\n";
    _display.DrawString(glm::ivec2(20, 20), planetInfo, 16.f);

    // HACK as the zone from which we want to draw the arrow is not loaded, the debug draw function doesn't work.
    // Need to fix in the renderer to draw scenes that don't have loaded zones.
    const WorldObjectID localPlayer = _world->GetPlayerHandler()->GetLocalPlayers()[0];
    const WorldObject* player = _world->GetWorldObject(localPlayer);
    assert(player != nullptr);

    constexpr f32 sphereRadius = 20.f;
    constexpr f32 extraSphereOffset = 2.f;

    for (const Planet::Biome& biome : planet->m_Biomes)
    {
        _display.DebugDrawSphere(
                player->GetWorldPosition().m_ZoneCoordinates,
                WorldPosition(planet->m_OriginZone, glm::vec3(0.f)).GetPositionRelativeTo(player->GetWorldPosition().m_ZoneCoordinates) + (planet->m_Radius + extraSphereOffset) * biome.m_BiomeDirection,
                sphereRadius,
                biome.m_GroundColor
                );
    }

    std::vector<std::pair<f32, const Planet::Biome*>> biomes;
    TerrainGeneration::GetClosestBiomes(*planet, player->GetWorldPosition(), biomes);

    u32 yOffset = 100;
    for (const auto& biome : biomes)
    {
        _display.DrawString(glm::ivec2(20, yOffset), "Biome weight: " + std::to_string(biome.first) + ". Tree cover: " + std::to_string(biome.second->m_TreeCoverage), 16.f, biome.second->m_GroundColor);

        yOffset += 20;
    }

    const Planet::BlendableBiomeInputs inputs = TerrainGeneration::BlendBiomeInputs(biomes);

    u32 octaveIdx = 1;
    for (const f32 weight : inputs.m_OctaveWeights)
    {
        _display.DrawString(
                glm::ivec2(20, yOffset),
                "Octave: " + std::to_string(octaveIdx) + " Weight: " + std::to_string(weight),
                16.f,
                Color(0.f, 0.f, 0.f, 1.f),
                FontStyle::Sans);

        yOffset += 20;
        ++octaveIdx;
    }
}
