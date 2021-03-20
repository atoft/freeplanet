/*
 * Copyright 2017-2021 Alastair Toft
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

#include <glm/glm.hpp>

#include <src/engine/inspection/contexts/InspectionContext.h>
#include <src/engine/inspection/BaseInspects.h>
#include <src/tools/globals.h>
#include <src/world/terrain/TerrainSubstance.h>

class BoxTerrainElement
{
public:
    BoxTerrainElement() = default;
    BoxTerrainElement(const glm::vec3& _position, const glm::vec3& _extents, f32 _transitionSize);

    f32 GetDensity(glm::vec3 position) const;
    TerrainSubstanceType GetSubstance() const { return m_Substance; };

    glm::vec3 m_Position;
    glm::vec3 m_Extents;
    TerrainSubstanceType m_Substance = TerrainSubstanceType::Rock;
};


template <typename InspectionContext>
void Inspect(std::string _name, BoxTerrainElement& _target, InspectionContext& _context)
{
    _context.Struct("BoxTerrainElement", InspectionType::Invalid, 0);
    Inspect("Position", _target.m_Position, _context);
    Inspect("Extents", _target.m_Extents, _context);
    Inspect("Substance", _target.m_Substance, _context);
    _context.EndStruct();
}

