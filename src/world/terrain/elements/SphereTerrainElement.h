//
// Created by alastair on 24/02/18.
//

#pragma once

#include <glm/vec3.hpp>
#include <src/tools/globals.h>

class SphereTerrainElement
{
protected:
    glm::vec3 m_Origin;
    float m_Radius;

public:
    SphereTerrainElement(glm::vec3 _origin, float _radius);

    f32 GetDensity(glm::vec3 _position) const;
};
