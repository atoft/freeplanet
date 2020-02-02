//
// Created by alastair on 24/02/18.
//

#pragma once

#include <glm/vec3.hpp>

#include <src/world/GeometryTypes.h>

class TerrainElement
{
protected:
    glm::vec3 m_Origin;

    AABB m_AABB;

public:
    virtual float GetDensity(glm::vec3 _position, glm::vec3 _globalOffset) const = 0;

};
