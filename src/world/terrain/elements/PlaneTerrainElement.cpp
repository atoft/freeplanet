//
// Created by alastair on 25/02/18.
//

#include "PlaneTerrainElement.h"

PlaneTerrainElement::PlaneTerrainElement(f32 _height)
    : m_Height(_height) {}

float PlaneTerrainElement::GetDensity(glm::vec3 _position, glm::vec3 _globalOffset) const
{
    return m_Height - _position.y - _globalOffset.y;
}


