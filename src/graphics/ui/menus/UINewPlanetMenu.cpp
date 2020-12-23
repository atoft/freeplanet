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

#include "UINewPlanetMenu.h"

#include <src/graphics/ui/menus/UIConstants.h>

void UINewPlanetMenu::Init(bool _startFocused)
{
    m_List.AddTextInput("");
    m_List.AddButton("Create", [this](UIActions& actions){actions.CreatePlanetFromSeed(std::get<UITextInput>(m_List.m_Widgets[0]).GetText());});
    m_List.AddButton("Back", [](UIActions& actions){actions.GoToMainMenu();});

    if (_startFocused)
    {
        m_List.Focus();
    }
}

void UINewPlanetMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    _display.FillScreen(UIConstants::BackgroundColor);
    _display.DrawString(UIConstants::ListStartPosition - glm::ivec2(0, 200), "New planet", 48.f);
    _display.DrawString(UIConstants::ListStartPosition - glm::ivec2(0, 44), "Enter a seed:", 24.f);
    m_List.Draw(_delta, _display);

}

void UINewPlanetMenu::OnButtonReleased(InputType _type, UIActions& _actions)
{
    m_List.OnButtonReleased(_type, _actions);
}

void UINewPlanetMenu::OnTextEntered(std::string _text)
{
    // TODO A nice way to access widgets..?
    std::get<UITextInput>(m_List.m_Widgets[0]).OnTextEntered(_text);
}

void UINewPlanetMenu::OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y)
{
    m_List.OnMouseHover(_display, _x, _y);
}
