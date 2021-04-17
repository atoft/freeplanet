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

#include <src/graphics/ui/UIBaseMenu.h>
#include <src/graphics/ui/widgets/UITextInput.h>

class UIConsoleMenu : public UIBaseMenu
{
public:
    UIConsoleMenu();
    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
    void OnTextEntered(std::string _text) override;
    void OnButtonReleased(const UIDrawInterface& _display, InputType _type, UIActions& _actions) override;

private:
    UITextInput m_Textbox;
    std::vector<std::string> m_HistoryBuffer;
    u32 m_HistoryIdx = 0;

    static constexpr u32 CONSOLE_HISTORY_SIZE = 32;
};
