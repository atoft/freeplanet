//
// Created by alastair on 24/02/18.
//

#pragma once

#include "TerrainElement.h"

#include <glm/vec3.hpp>

class SphereTerrainElement : public TerrainElement
{
protected:
    float m_Radius;

public:
    SphereTerrainElement(glm::vec3 _origin, float _radius);

    f32 GetDensity(glm::vec3 _position, glm::vec3 _globalOffset) const override;
};
