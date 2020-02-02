//
// Created by alastair on 01/10/19.
//


#pragma once

#include <glm/glm.hpp>

#include <src/tools/globals.h>

class BoxTerrainElement
{
public:
    BoxTerrainElement(const glm::vec3& _position, const glm::vec3& _extents, f32 _transitionSize);

    f32 GetDensity(glm::vec3 position, glm::vec3 globalOffset) const;

private:
    glm::vec3 m_Position;
    glm::vec3 m_Extents;
};



