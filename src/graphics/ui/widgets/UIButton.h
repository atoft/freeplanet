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

#include <functional>

#include <src/graphics/ui/UIDrawInterface.h>

class UIActions;

class UIButton
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, bool _isFocused, bool _isActive);
    void OnPressed(UIActions& _actions);
    bool IsHovered(const UIDrawInterface& _display, f32 _x, f32 _y) const;
    void OnTextEntered(std::string _text) {};

public:
    std::string m_Label;
    glm::ivec2 m_Position = glm::ivec2();
    glm::ivec2 m_Dimensions = glm::ivec2();

    std::function<void(UIActions&)> m_OnPressed;
};
