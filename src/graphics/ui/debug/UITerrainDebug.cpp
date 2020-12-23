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
