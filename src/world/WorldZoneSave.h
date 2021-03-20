/*
 * Copyright 2021 Alastair Toft
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
#include <vector>

#include <src/tools/globals.h>
#include <src/engine/InputTypes.h>
#include <src/engine/inspection/contexts/InspectionContext.h>
#include <src/engine/inspection/BaseInspects.h>
#include <src/world/terrain/TerrainEdits.h>

struct WorldZoneSave
{
    glm::ivec3 m_ZoneCoords;

    TerrainEdits m_TerrainEdits;
};

template <typename InspectionContext>
void Inspect(std::string _name, WorldZoneSave& _target, InspectionContext& _context)
{
    _context.Struct("WorldZoneSave", InspectionType::Invalid, 0);
    Inspect("ZoneCoords", _target.m_ZoneCoords, _context);
    Inspect("TerrainEdits", _target.m_TerrainEdits, _context);
    _context.EndStruct();
}

