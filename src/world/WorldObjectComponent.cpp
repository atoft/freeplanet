//
// Created by alastair on 14/09/17.
//

#include "WorldObjectComponent.h"

#include <src/world/WorldObject.h>
#include <src/world/World.h>

WorldObject* WorldObjectComponent::GetOwnerObject()
{
    return m_World->GetWorldObject(m_WorldObjectID);
}

const WorldObject* WorldObjectComponent::GetOwnerObject() const
{
    return m_World->GetWorldObject(m_WorldObjectID);
}
