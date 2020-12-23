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

#include <src/graphics/ui/menus/UIConstants.h>
#include "UIList.h"

void UIList::AddButton(std::string _label, std::function<void(UIActions&)> _onPressed)
{
    UIButton button;
    button.m_Label = _label;
    button.m_OnPressed = _onPressed;
    button.m_Position = UIConstants::ListStartPosition + glm::ivec2(0, UIConstants::ListItemSpacing) * static_cast<s32>(m_Widgets.size());
    button.m_Dimensions = UIConstants::ButtonDimensions;
    m_Widgets.push_back(button);
}

void UIList::AddTextInput(std::string _label)
{
    UITextInput input;
    input.m_Position = UIConstants::ListStartPosition + glm::ivec2(0, UIConstants::ListItemSpacing) * static_cast<s32>(m_Widgets.size());
    input.m_Dimensions = UIConstants::ButtonDimensions;
    m_Widgets.push_back(input);
}

void UIList::OnButtonReleased(InputType _type, UIActions& _actions)
{
    if (m_Widgets.empty())
    {
        return;
    }

    switch (_type)
    {
    case InputType::MenuDown:
    {
        if (m_ActiveIndex != std::nullopt)
        {
            m_ActiveIndex = (*m_ActiveIndex + 1) % m_Widgets.size();
        }
        else
        {
            m_ActiveIndex = 0;
        }

        break;
    }
    case InputType::MenuUp:
    {
        if (m_ActiveIndex != std::nullopt)
        {
            m_ActiveIndex = (*m_ActiveIndex == 0) ? m_Widgets.size() - 1 : *m_ActiveIndex - 1;
        }
        else
        {
            m_ActiveIndex = 0;
        }

        break;
    }
    case InputType::MenuAccept:
    {
        if (m_ActiveIndex != std::nullopt)
        {
            std::visit([&_actions](auto&& value){value.OnPressed(_actions);}, m_Widgets[*m_ActiveIndex]);
        }

        break;
    }
    default:
        break;
    }
}

void UIList::OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y)
{
    m_ActiveIndex = std::nullopt;

    u32 index = 0;
    for (const UIWidgetVariant& widget : m_Widgets)
    {
        const bool isHovered = std::visit([&_display, _x, _y](auto&& value){return value.IsHovered(_display, _x, _y);}, widget);

        if (isHovered)
        {
            m_ActiveIndex = index;
            break;
        }

        ++index;
    }
}

void UIList::Draw(TimeMS _delta, UIDrawInterface& _display)
{
    u32 index = 0;
    for (UIWidgetVariant & widget : m_Widgets)
    {
        std::visit([&](auto&& value){value.Draw(_delta, _display, index == m_ActiveIndex);}, widget);

        ++index;
    }
}
