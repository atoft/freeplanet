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

#include "UIHUDMenu.h"

#include <src/graphics/ui/menus/UIConstants.h>
#include <src/world/inventory/InventoryHandler.h>
#include <src/world/World.h>

void UIHUDMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    const InventoryHandler* inventoryHandler = _world->GetInventoryHandler();

    constexpr u32 PLAYER_IDX = 0;
    const Inventory& inventory = inventoryHandler->GetInventory(PLAYER_IDX);

    f32 currentX = UIConstants::ScreenEdgeMargin.x;

    constexpr f32 UNSELECTED_WIDTH = 100.f;
    constexpr f32 SELECTED_WIDTH = 200.f;
    constexpr f32 MARGIN_WDITH = 20.f;

    m_TimeSinceChange += _delta;

    constexpr f32 COOLDOWN_TIME = 1000.f;
    constexpr f32 FADE_DURATION = 500.f;

    if (m_TimeSinceChange > COOLDOWN_TIME + FADE_DURATION)
    {
        return;
    }

    const f32 alpha = 1.f - glm::clamp((m_TimeSinceChange - COOLDOWN_TIME) / FADE_DURATION, 0.f, 1.f);

    const Color drawColor = Color(1.f, 1.f, 1.f, alpha);

    u32 slotIdx = 0;
    for (const InventorySlot& slot : inventory.m_Slots)
    {
        const bool isSelected = slotIdx == inventory.m_SelectedIndex;

        if (isSelected)
        {
            _display.DrawString(glm::vec2(currentX, SELECTED_WIDTH + UIConstants::ScreenEdgeMargin.y), slot.m_DisplayName, 24.f, drawColor);
        }

        _display.DrawSprite(glm::vec2(currentX, UIConstants::ScreenEdgeMargin.y), glm::vec2(isSelected ? SELECTED_WIDTH : UNSELECTED_WIDTH), slot.m_Icon, drawColor);

        currentX += (isSelected ? SELECTED_WIDTH : UNSELECTED_WIDTH) + MARGIN_WDITH;

        ++slotIdx;
    }
}

void UIHUDMenu::OnButtonReleased(const UIDrawInterface& _display, InputType _type, UIActions& _actions)
{
    if (_type == InputType::InventorySlot1
     || _type == InputType::InventorySlot2
     || _type == InputType::InventorySlot3
     || _type == InputType::InventorySlot4
     || _type == InputType::InventorySlot5)
    {
        m_TimeSinceChange = 0.f;
    }
}
