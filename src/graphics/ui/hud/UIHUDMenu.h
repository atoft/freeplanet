/*
 * Copyright 2020 Alastair Toft
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <src/graphics/ui/UIBaseMenu.h>

class UIHUDMenu : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
    void OnButtonReleased(InputType _type, UIActions& _actions) override;

private:
    AssetHandle<UITexture> m_TestTexture = AssetHandle<UITexture>(TextureAsset_Dev_512);

    float m_TimeSinceChange = 0.f;
};
