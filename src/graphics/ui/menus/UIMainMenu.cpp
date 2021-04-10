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

#include "UIMainMenu.h"

#include <src/config.h>
#include <src/graphics/ui/UIActions.h>
#include <src/graphics/ui/menus/UIConstants.h>

void UIMainMenu::Init(bool _startFocused)
{
    m_List.AddButton("New Planet", [](UIActions& actions) { actions.GoToNewPlanetMenu(); });
    m_List.AddButton("Load Planet", [](UIActions& actions) { actions.GoToLoadPlanetMenu(); });
    m_List.AddButton("Quit", [](UIActions& actions) { actions.Quit(); });

    if (_startFocused)
    {
        m_List.Focus();
    }
}

void UIMainMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    const std::string buildInfo = freeplanet_APP_NAME_PRETTY " " + std::to_string(freeplanet_VERSION_MAJOR) + "."
        + std::to_string(freeplanet_VERSION_MINOR);

    _display.FillScreen(UIConstants::BackgroundColor);
    _display.DrawString(UIConstants::ListStartPosition - glm::ivec2(0, 200), "freeplanet", 48.f);
    _display.DrawString(glm::ivec2(UIConstants::ScreenEdgeMargin.x,
                                   UIConstants::UIResolution.y - UIConstants::ScreenEdgeMargin.y - 12.f),
                        buildInfo,
                        12.f,
                        Color(1.f),
                        FontStyle::Monospace,
                        UIAnchorPosition::BottomLeft);

    m_List.Draw(_delta, _display);
}

void UIMainMenu::OnButtonReleased(InputType _type, UIActions& _actions)
{
    m_List.OnButtonReleased(_type, _actions);
}

void UIMainMenu::OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y)
{
    m_List.OnMouseHover(_display, _x, _y);
}
