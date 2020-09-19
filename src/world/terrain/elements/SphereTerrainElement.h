//
// Created by alastair on 24/02/18.
//

#pragma once

#include <glm/vec3.hpp>
#include <src/tools/globals.h>
#include <src/world/terrain/TerrainSubstance.h>

class SphereTerrainElement
{
public:
    SphereTerrainElement(glm::vec3 _origin, float _radius, TerrainSubstanceType _substance);

    f32 GetDensity(glm::vec3 _position) const;
    TerrainSubstanceType GetSubstance() const { return m_Substance; };

private:
    glm::vec3 m_Origin;
    float m_Radius;
    TerrainSubstanceType m_Substance = TerrainSubstanceType::Rock;
};
