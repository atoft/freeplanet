//
// Created by alastair on 14/09/17.
//

#include "WorldObjectComponent.h"

#include <src/world/WorldObject.h>
#include <src/world/World.h>

WorldObject* WorldObjectComponent::GetOwnerObject()
{
    WorldObject* worldObject = m_World->GetWorldObject(m_WorldObjectID);

    if (worldObject == nullptr)
    {
        LogError("A component is missing its owner ID" + std::to_string(m_WorldObjectID));
        assert(false);
    }

    return worldObject;
}

const WorldObject* WorldObjectComponent::GetOwnerObject() const
{
    const WorldObject* worldObject = m_World->GetWorldObject(m_WorldObjectID);

    if (worldObject == nullptr)
    {
        LogError("A component is missing its owner ID" + std::to_string(m_WorldObjectID));
        assert(false);
    }

    return worldObject;
}
