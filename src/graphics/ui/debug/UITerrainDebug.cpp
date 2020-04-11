//
// Created by alastair on 03/03/19.
//

#include "UITerrainDebug.h"

#include <src/tools/MathsHelpers.h>
#include <src/world/terrain/TerrainHandler.h>
#include <src/world/World.h>

void UITerrainDebug::Draw(TimeMS, UIDrawInterface& _display, const World* _world)
{
    if (_world == nullptr)
    {
        return;
    }

    for (const WorldZone& zone : _world->GetActiveZones())
    {
        for (const BipedComponent& component : zone.GetComponents<BipedComponent>())
        {
            std::string displayString;

            const TerrainHandler* terrainHandler = _world->GetTerrainHandler();
            assert(terrainHandler != nullptr);

            displayString += "Density at player: " + std::to_string(terrainHandler->GetDensity(component.GetOwnerObject()->GetWorldPosition()));

            _display.DrawString(glm::ivec2(20, 20), displayString, 24.f);

            // Only bother with the first biped component.
            // Debug displays will need fixing when there are multiple players.
            break;
        }
    }
}
