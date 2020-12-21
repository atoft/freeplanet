//
// Created by alastair on 26/08/17.
//

#pragma once

#include <glm/vec3.hpp>
#include "WorldObjectComponent.h"

struct LightEmitter
{
    Color m_Color;
    f32 m_Brightness;
    glm::vec3 m_LocalOffset;
};

class LightComponent : public WorldObjectComponent
{
public:
    LightComponent(World* _world, WorldObjectID _ownerID);

    LightEmitter m_Emitter;
};

