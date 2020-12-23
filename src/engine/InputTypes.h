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

#include <string>

#include <SFML/Window/Mouse.hpp>
#include <src/tools/globals.h>
#include <src/engine/KeyboardKey.h>

class InspectionContext;

enum class InputType
{
    None = 0,
    MoveForward,
    MoveBack,
    TurnLeft,
    TurnRight,
    LookUp,
    LookDown,
    StrafeLeft,
    StrafeRight,
    Jump,
    Sprint,

    Interact,
    InteractAlternate,
    InventorySlot1,
    InventorySlot2,
    InventorySlot3,
    InventorySlot4,
    InventorySlot5,

    LevitateUp,
    LevitateDown,
    RollLeft,
    RollRight,

    ToggleLog,
    MenuAccept,
    MenuBack,
    MenuLeft,
    MenuRight,
    MenuUp,
    MenuDown,
    EngineQuit,

    Count
};

enum class InputButtonInteraction
{
    None,
    IsDown,
    OnPressed,
    OnReleased,

    Count
};

enum class InputContext
{
    None,
    UI,
    Gameplay,
    Debug,

    Count
};

struct InputKeyMapping
{
    InputType m_InputType = InputType::None;
    InputContext m_InputContext = InputContext::None;

    KeyboardKey m_Key = KeyboardKey::Unknown;
    InputButtonInteraction m_Interaction = InputButtonInteraction::None;
};

struct InputMouseButtonMapping
{
    InputType m_InputType = InputType::None;
    InputContext m_InputContext = InputContext::None;

    sf::Mouse::Button m_Button = sf::Mouse::Button::ButtonCount;
    InputButtonInteraction m_Interaction = InputButtonInteraction::None;
};

std::string ToString(InputType _inputType);
std::string ToString(InputContext _inputContext);
std::string ToString(InputButtonInteraction _interaction);
std::string ToString(KeyboardKey _key);

void Inspect(std::string _name, InputKeyMapping& _target, InspectionContext& _context);
