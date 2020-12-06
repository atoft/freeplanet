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

#pragma once

#include <vector>

#include <src/engine/AssetHandle.h>
#include <src/world/terrain/TerrainSubstance.h>
#include <src/tools/globals.h>
#include <src/tools/PropRecipe.h>

class UITexture;

struct InventorySlot
{
    std::optional<TerrainSubstanceType> m_Substance;
    u32 m_Count = 100;
    AssetHandle<UITexture> m_Icon;
    std::optional<PropRecipe> m_Prop;
};

struct Inventory
{
    u32 m_PlayerID = 0;
    std::vector<InventorySlot> m_Slots;
    u32 m_SelectedIndex = 0;
};
