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

#include <src/tools/globals.h>

class World;
class WorldObject;
enum class InputType;

class CameraHandler
{
public:
    explicit CameraHandler(World* _world);
    void Update(TimeMS _delta);
    void OnMouseInput(u32 _playerIdx, WorldObject* _controlledWorldObject, f32 _x, f32 _y);
    
private:
    World* m_World = nullptr;
};
