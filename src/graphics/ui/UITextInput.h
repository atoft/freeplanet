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

#include <src/tools/globals.h>

class UIDrawInterface;
class UIActions;

class UITextInput
{
public:
    void OnTextEntered(std::string _text);
    const std::string& GetText() { return m_InputBuffer;};
    void SetText(const std::string& _text);
    void Clear();
    void MoveCaretLeft();
    void MoveCaretRight();

    void Draw(TimeMS _delta, UIDrawInterface& _display, bool _isFocused);
    void OnPressed(UIActions& _actions) {};

    // TODO support focus for TextInput.
    bool IsHovered(const UIDrawInterface& _display, f32 _x, f32 _y) const { return false; };

public:
    glm::ivec2 m_Position = glm::ivec2();
    glm::ivec2 m_Dimensions = glm::ivec2();
    f32 m_FontSize = 32.f;

private:
    std::string m_InputBuffer;
    u32 m_CaretPosition = 0;
};
