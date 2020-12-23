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

#include "UIButton.h"

void UIButton::Draw(TimeMS _delta, UIDrawInterface& _display, bool _isFocused)
{
    constexpr Color inactiveColor = Color(0.843f, 0.941f, 0.956f, 1.f);
    constexpr Color activeColor = Color(0.525f, 0.905f, 0.976f, 1.f);
    constexpr Color fontColor = Color(0.294f, 0.439f, 0.466f, 1.f);
    constexpr f32 fontSize = 16.f;

    _display.DrawRectangle(m_Position, m_Dimensions, _isFocused ? activeColor : inactiveColor);

    const glm::ivec2 textOffset = glm::ivec2((m_Dimensions.y - fontSize) / 2.f);
    _display.DrawString(m_Position + textOffset, m_Label, fontSize, fontColor);
}

void UIButton::OnPressed(UIActions& _actions)
{
    m_OnPressed(_actions);
}

bool UIButton::IsHovered(const UIDrawInterface& _display, f32 _x, f32 _y) const
{
    return _display.IsInAABB(glm::uvec2(_x, _y), m_Position, m_Dimensions, UIAnchorPosition::Centered);
}
