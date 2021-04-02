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

struct WorldSave
{
    std::string m_Name;
    bool m_HasPlanet = true;
    u32 m_Seed = 0;
};

template <typename InspectionContext>
void Inspect(std::string _name, WorldSave& _target, InspectionContext& _context)
{
    _context.Struct("WorldSave", InspectionType::Invalid, 0);
    // TODO Support string inspection Inspect("Name", _target.m_Name, _context);
    Inspect("HasPlanet", _target.m_HasPlanet, _context);
    Inspect("Seed", _target.m_Seed, _context);
    _context.EndStruct();
}