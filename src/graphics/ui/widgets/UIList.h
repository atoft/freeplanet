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

#include <optional>
#include <variant>

#include <src/graphics/ui/widgets/UIButton.h>
#include <src/engine/InputTypes.h>
#include <src/graphics/ui/UITextInput.h>

using UIWidgetVariant = std::variant<UIButton, UITextInput>;

class UIList
{
public:
    void AddButton(std::string _label, std::function<void(UIActions&)> _onPressed);
    void AddTextInput(std::string _label);

    void OnButtonReleased(InputType _type, UIActions& _actions);
    void OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y);

    void Draw(TimeMS _delta, UIDrawInterface& _display);
    void Focus() { m_ActiveIndex = 0; };

public:
    std::vector<UIWidgetVariant> m_Widgets;

private:
    std::optional<u32> m_ActiveIndex;
};
