//
// Created by alastair on 26/08/17.
//

#pragma once

#include <glm/vec3.hpp>
#include "WorldObjectComponent.h"

class LightComponent : public WorldObjectComponent
{
public:
    LightComponent(glm::vec3 color, f32 brightness);

    const glm::vec3 GetColor() const;

    f32 GetBrightness() const;

private:
    glm::vec3 m_Color;
    float m_Brightness;
};

