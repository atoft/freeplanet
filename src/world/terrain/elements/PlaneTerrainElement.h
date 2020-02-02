//
// Created by alastair on 25/02/18.
//

#pragma once

#include "TerrainElement.h"

class PlaneTerrainElement : public TerrainElement
{
protected:
    f32 m_Height;

public:
    PlaneTerrainElement(f32 _height);

    f32 GetDensity(glm::vec3 _position, glm::vec3 _globalOffset) const override;
};
