//
// Created by alastair on 30/07/19.
//

#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

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
    OnReleased
};

enum class InputContext
{
    None,
    UI,
    Gameplay,
    Debug
};

struct InputKeyMapping
{
    InputType m_InputType = InputType::None;
    InputContext m_InputContext = InputContext::None;

    sf::Keyboard::Key m_Key = sf::Keyboard::Key::Unknown;
    InputButtonInteraction m_Interaction = InputButtonInteraction::None;
};

struct InputMouseButtonMapping
{
    InputType m_InputType = InputType::None;
    InputContext m_InputContext = InputContext::None;

    sf::Mouse::Button m_Button = sf::Mouse::Button::ButtonCount;
    InputButtonInteraction m_Interaction = InputButtonInteraction::None;
};