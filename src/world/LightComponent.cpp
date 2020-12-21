//
// Created by alastair on 26/08/17.
//

#include "LightComponent.h"

LightComponent::LightComponent(World* _world, WorldObjectID _ownerID)
{
    m_World = _world;
    m_WorldObjectID = _ownerID;
}
