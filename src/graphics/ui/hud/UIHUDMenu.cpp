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

#include "UIHUDMenu.h"

#include <src/world/inventory/InventoryHandler.h>
#include <src/world/World.h>

void UIHUDMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    const InventoryHandler* inventoryHandler = _world->GetInventoryHandler();

    constexpr u32 PLAYER_IDX = 0;
    const Inventory& inventory = inventoryHandler->GetInventory(PLAYER_IDX);

    u32 slotIdx = 0;
    for (const InventorySlot& slot : inventory.m_Slots)
    {
        const bool isSelected = slotIdx == inventory.m_SelectedIndex;

        _display.DrawString(glm::vec2(20,40 * slotIdx), (isSelected ? ">" : "-") + ToString(slot.m_Substance), 24.f);

        ++slotIdx;
    }
}