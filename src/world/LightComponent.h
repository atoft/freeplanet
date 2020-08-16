//
// Created by alastair on 26/08/17.
//

#pragma once

#include <glm/vec3.hpp>
#include "WorldObjectComponent.h"

class LightComponent : public WorldObjectComponent
{
public:
    LightComponent(World* _world, WorldObjectID _ownerID, Color _color, f32 _brightness);

    Color m_Color;
    f32 m_Brightness;
};

