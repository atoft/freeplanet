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

#include <src/engine/InputTypes.h>
#include <src/world/inventory/InventoryTypes.h>

class World;

class InventoryHandler
{
public:
    explicit InventoryHandler(World* _world);
    void RegisterLocalPlayer(u32 _playerIndex);
    void OnButtonInput(InputType _inputType);

private:
    void ChangeSlot(u32 _playerIndex, u32 _slotIndex);

private:
    std::vector<Inventory> m_Inventories;
    World* m_World = nullptr;
};