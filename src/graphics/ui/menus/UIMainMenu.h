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

#include <src/graphics/ui/UIBaseMenu.h>
#include <src/graphics/ui/widgets/UIList.h>

class UIMainMenu : public UIBaseMenu
{
public:
    void Init(bool _startFocused) override;

    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
    void OnButtonReleased(const UIDrawInterface& _display, InputType _type, UIActions& _actions) override;
    void OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y) override;
    bool ShouldTakeFocus() const override { return true; };

private:
    UIList m_List;
};
