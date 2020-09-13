//
// Created by alastair on 24/02/18.
//

#include "SphereTerrainElement.h"

#include <glm/glm.hpp>

SphereTerrainElement::SphereTerrainElement(glm::vec3 _origin, f32 _radius)
{
    m_Origin = _origin;
    m_Radius = _radius;
}

f32 SphereTerrainElement::GetDensity(glm::vec3 _position) const
{
    return m_Radius - glm::length(_position - m_Origin);
}


