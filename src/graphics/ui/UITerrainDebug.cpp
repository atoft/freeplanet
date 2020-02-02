//
// Created by alastair on 03/03/19.
//

#include "UITerrainDebug.h"

#include <src/tools/MathsHelpers.h>
#include <src/world/terrain/TerrainHandler.h>
#include <src/world/World.h>

void UITerrainDebug::Init(const World* _world)
{
    m_World = _world;
}

void UITerrainDebug::Draw(TimeMS _delta, UIDrawInterface& _display)
{
    if (m_World == nullptr)
    {
        return;
    }

    for (const WorldZone& zone : m_World->GetActiveZones())
    {
        for (const BipedComponent& component : zone.GetBipedComponents())
        {
            std::string displayString;

            const TerrainHandler* terrainHandler = m_World->GetTerrainHandler();
            assert(terrainHandler != nullptr);

            displayString += "Density at player: " + std::to_string(terrainHandler->GetDensity(component.GetOwnerObject()->GetWorldPosition()));

            _display.DrawString(glm::ivec2(20, 20), displayString, 24.f);

            // Only bother with the first biped component.
            // Debug displays will need fixing when there are multiple players.
            break;
        }
    }
}
