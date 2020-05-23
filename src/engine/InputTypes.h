//
// Created by alastair on 30/07/19.
//

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