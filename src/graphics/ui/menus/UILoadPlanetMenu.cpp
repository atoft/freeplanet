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

#include "UILoadPlanetMenu.h"

#include <src/graphics/ui/menus/UIConstants.h>
#include <src/world/World.h>

void UILoadPlanetMenu::Init(bool _startFocused)
{
    const std::vector<std::string> worldNames = World::GetSavedWorldNames();

    for (const std::string& name : worldNames)
    {
        m_List.AddButton(name, [name](UIActions& actions)
        {
            actions.LoadPlanetFromName(name);
        });
    }

    m_List.AddButton("Back", [](UIActions& actions){actions.GoToMainMenu();});

    if (_startFocused)
    {
        m_List.Focus();
    }
}

void UILoadPlanetMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    _display.FillScreen(UIConstants::BackgroundColor);
    _display.DrawString(UIConstants::ListStartPosition - glm::ivec2(0, 200), "Load planet", 48.f);
    m_List.Draw(_delta, _display);

}

void UILoadPlanetMenu::OnButtonReleased(InputType _type, UIActions& _actions)
{
    m_List.OnButtonReleased(_type, _actions);
}

void UILoadPlanetMenu::OnTextEntered(std::string _text)
{
    // TODO A nice way to access widgets..?
    std::get<UITextInput>(m_List.m_Widgets[0]).OnTextEntered(_text);
}

void UILoadPlanetMenu::OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y)
{
    m_List.OnMouseHover(_display, _x, _y);
}
