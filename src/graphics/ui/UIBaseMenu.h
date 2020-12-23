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

#include <src/graphics/ui/UIDisplay.h>
#include <src/engine/InputTypes.h>
#include <src/tools/globals.h>

class UIDrawInterface;
class UIActions;
class World;

class UIBaseMenu
{
public:
    virtual void Init(bool _startFocused) {};
    virtual void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) = 0;
    virtual void OnTextEntered(std::string _text) {};
    virtual void OnButtonReleased(InputType _type, UIActions& _actions) {};
    virtual void OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y) {};

    virtual bool ShouldTakeFocus() const { return false; };
    virtual ~UIBaseMenu() = default;
};

