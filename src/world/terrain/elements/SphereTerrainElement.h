/*
 * Copyright 2017-2020 Alastair Toft
 *
 * This file is part of freeplanet.
 *
 * freeplanet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeplanet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeplanet. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <glm/vec3.hpp>
#include <src/tools/globals.h>
#include <src/world/terrain/TerrainSubstance.h>

#include <src/engine/inspection/contexts/InspectionContext.h>
#include <src/engine/inspection/BaseInspects.h>

class SphereTerrainElement
{
public:
    SphereTerrainElement() = default;
    SphereTerrainElement(glm::vec3 _origin, float _radius, TerrainSubstanceType _substance);

    f32 GetDensity(glm::vec3 _position) const;
    TerrainSubstanceType GetSubstance() const { return m_Substance; };

    glm::vec3 m_Origin;
    float m_Radius;
    TerrainSubstanceType m_Substance = TerrainSubstanceType::Rock;
};

template <typename InspectionContext>
void Inspect(std::string _name, SphereTerrainElement& _target, InspectionContext& _context)
{
    _context.Struct("SphereTerrainElement", InspectionType::Invalid, 0);
    Inspect("Position", _target.m_Origin, _context);
    Inspect("Radius", _target.m_Radius, _context);
    Inspect("Substance", _target.m_Substance, _context);
    _context.EndStruct();
}

