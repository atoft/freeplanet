//
// Created by alastair on 26/08/17.
//

#include "LightComponent.h"

f32 LightComponent::GetBrightness() const
{
    return m_Brightness;
}

const glm::vec3 LightComponent::GetColor() const
{
    return m_Color;
}

LightComponent::LightComponent(glm::vec3 color, f32 brightness)
{
    //TODO: Perform sanity checks on these values
    m_Color = color;
    m_Brightness = brightness;
}
