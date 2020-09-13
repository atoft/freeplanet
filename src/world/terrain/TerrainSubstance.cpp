/* 
 * Copyright 2020 Alastair Toft
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TerrainSubstance.h"

#include <src/tools/globals.h>

TerrainSubstance TerrainSubstance::FromType(TerrainSubstanceType _type)
{
    switch (_type)
    {
        case TerrainSubstanceType::Topsoil:
            return {1.f, 0.f, 0.f, 0.f};
        case TerrainSubstanceType::Dirt:
            return {0.f, 1.f, 0.f, 0.f};
        case TerrainSubstanceType::Rock:
            return {0.f, 0.f, 1.f, 0.f};
        case TerrainSubstanceType::Sand:
        default:
            return {0.f, 0.f, 0.f, 1.f};
    }
    static_assert(static_cast<u32>(TerrainSubstanceType::Count) == 4);
}
