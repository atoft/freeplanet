//
// Created by alastair on 04/04/2020.
//

#include "UIPlanetDebug.h"

#include <src/world/planet/Planet.h>
#include <src/world/World.h>

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

    constexpr f32 arrowExtraLength = 16.f;

    // HACK as the zone from which we want to draw the arrow is not loaded, the debug draw function doesn't work.
    // Need to fix in the renderer to draw scenes that don't have loaded zones.
    const WorldObjectID localPlayer = _world->GetLocalPlayers()[0];
    const WorldObject* player = _world->GetWorldObject(localPlayer);
    assert(player != nullptr);

    for (const Planet::Biome& biome : planet->m_Biomes)
    {
        _display.DebugDrawArrow(
                player->GetWorldPosition().m_ZoneCoordinates,
                WorldPosition(planet->m_OriginZone, glm::vec3(0.f)).GetPositionRelativeTo(player->GetWorldPosition().m_ZoneCoordinates),
                (planet->m_Radius + arrowExtraLength),
                biome.m_BiomeDirection);
    }
}
