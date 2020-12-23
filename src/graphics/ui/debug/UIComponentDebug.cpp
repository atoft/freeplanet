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

#include "UIComponentDebug.h"

#include <src/world/World.h>

void UIComponentDebug::Draw(TimeMS, UIDrawInterface& _display, const World* _world)
{
    if(_world == nullptr)
    {
        return;
    }

    // TODO Implement iterators over components for world
    for(const auto& zone : _world->GetActiveZones())
    {
        if (!_world->GetPlayerHandler()->IsPlayerInZone(zone.GetCoordinates()))
        {
            continue;
        }

        std::string objectList = "WorldObjects in this zone: \n";

        for (const auto& object : zone.GetWorldObjects())
        {
            objectList += object.GetName() + " (ID" + std::to_string(object.GetWorldObjectID()) + ")\n";
        }

        objectList += "\nMovable ColliderComponents in this zone: \n";
        u32 idx = 0;
        for (const auto& component : zone.GetComponents<ColliderComponent>())
        {
            if (component.m_MovementType != MovementType::Movable)
            {
                continue;
            }

            objectList += "Index " + std::to_string(idx) + " - Owner ID" + std::to_string(component.GetOwner()) + "\n";

            for (const auto& collision : component.m_CollisionsLastFrame)
            {
                objectList += glm::to_string(collision.m_Normal) + " - " + std::to_string(collision.m_Distance) + "\n";
            }

            ++idx;
        }

        _display.DrawString(glm::ivec2(20, 800), objectList, 16.f);
    }
}
