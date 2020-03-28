//
// Created by alastair on 17/12/18.
//

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
        if (!_world->IsPlayerInZone(zone.GetCoordinates()))
        {
            continue;
        }

        std::string objectList = "WorldObjects in this zone: \n";

        for (const auto& component : zone.GetBipedComponents())
        {
            component.DebugDraw(_display);
        }

        for (const auto& object : zone.GetWorldObjects())
        {
            objectList += object.GetName() + " (ID" + std::to_string(object.GetWorldObjectID()) + ")\n";
        }

        objectList += "\nMovable ColliderComponents in this zone: \n";
        u32 idx = 0;
        for (const auto& component : zone.GetColliderComponents())
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
