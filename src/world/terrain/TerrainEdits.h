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

#include <variant>
#include <vector>

#include <src/engine/inspection/contexts/InspectionContext.h>
#include <src/engine/inspection/BaseInspects.h>
#include <src/world/terrain/elements/BoxTerrainElement.h>
#include <src/world/terrain/elements/SphereTerrainElement.h>

using TerrainElementVariant = std::variant<SphereTerrainElement, BoxTerrainElement>;

struct TerrainEdits
{
    f32 GetDensity(glm::vec3 _localPosition) const;
    void GetSubstance(glm::vec3 _localPosition, TerrainSubstance& _inOutSubstance) const;
    bool IsEmpty() const;

    std::vector<TerrainElementVariant> m_AdditiveElements;
    std::vector<TerrainElementVariant> m_SubtractiveElements;
};


template <typename InspectionContext>
void Inspect(std::string _name, TerrainEdits& _target, InspectionContext& _context)
{
    _context.Struct("TerrainEdits", InspectionType::Invalid, 0);
    Inspect("AdditiveElements", _target.m_AdditiveElements, _context);
    Inspect("SubtractiveElements", _target.m_SubtractiveElements, _context);
    _context.EndStruct();
}


