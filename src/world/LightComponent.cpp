//
// Created by alastair on 26/08/17.
//

#include "LightComponent.h"

LightComponent::LightComponent(World* _world, WorldObjectID _ownerID, Color _color, f32 _brightness)
{
    m_World = _world;
    m_WorldObjectID = _ownerID;
    m_Color = _color;
    m_Brightness = _brightness;
}