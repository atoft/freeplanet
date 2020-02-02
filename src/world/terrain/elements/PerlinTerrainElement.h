//
// Created by alastair on 07/03/18.
//

#pragma once

#include "TerrainElement.h"

#include <src/tools/globals.h>

class PerlinTerrainElement : public TerrainElement
{
public:
    f32 GetDensity(glm::vec3 _position, glm::vec3 _globalOffset) const override;
};
