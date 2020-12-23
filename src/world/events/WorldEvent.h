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

#include <optional>

#include <src/world/WorldObjectID.h>
#include <src/world/WorldPosition.h>
#include <src/world/terrain/TerrainSubstance.h>

// Events that occur in the world. It's possible that there will be listeners
// outside the world (e.g. for playing world sounds or similar) but they should
// only be sent from within the world.
class WorldEvent
{
public:
    enum class Type
    {
        Invalid = 0,
        AddTerrain,
        RemoveTerrain
    };

    WorldObjectID m_Source = WORLDOBJECTID_INVALID;
    std::optional<WorldPosition> m_TargetPosition = std::nullopt;
    std::optional<f32> m_Radius = std::nullopt;
    std::optional<TerrainSubstanceType> m_Substance = std::nullopt;

    Type m_Type = Type::Invalid;
};
