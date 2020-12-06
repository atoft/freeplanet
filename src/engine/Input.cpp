//
// Created by alastair on 23/07/17.
//

#include "Input.h"
#include "src/engine/events/EngineEvent.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window.hpp>

#include <src/engine/Engine.h>
#include <src/graphics/ui/UIDisplay.h>
#include <src/world/World.h>

std::vector<InputKeyMapping> Input::DEFAULT_KEY_MAPPINGS
        {
                {InputType::EngineQuit, InputContext::Debug, KeyboardKey::Escape, InputButtonInteraction::OnReleased},
                {InputType::MenuLeft, InputContext::UI, KeyboardKey::Left, InputButtonInteraction::OnPressed},
                {InputType::MenuRight, InputContext::UI, KeyboardKey::Right, InputButtonInteraction::OnPressed},
                {InputType::MenuUp, InputContext::UI, KeyboardKey::Up, InputButtonInteraction::OnPressed},
                {InputType::MenuDown, InputContext::UI, KeyboardKey::Down, InputButtonInteraction::OnPressed},

                {InputType::MenuAccept, InputContext::UI, KeyboardKey::Enter, InputButtonInteraction::OnPressed},
                {InputType::ToggleLog, InputContext::UI, KeyboardKey::Tilde, InputButtonInteraction::OnPressed},

				// @CrossPlatform Grave/"UK tilde" is not recognised on Windows - at least with a UK Keyboard layout.
				// I think it's a bug or limitation in SFML and works by accident (?) on Linux.
				// Use F1 on Windows as a workaround.
				{InputType::ToggleLog, InputContext::UI, KeyboardKey::F1, InputButtonInteraction::OnPressed},

                {InputType::MoveForward, InputContext::Gameplay, KeyboardKey::W, InputButtonInteraction::IsDown},
                {InputType::MoveBack, InputContext::Gameplay, KeyboardKey::S, InputButtonInteraction::IsDown},
                {InputType::StrafeLeft, InputContext::Gameplay, KeyboardKey::A, InputButtonInteraction::IsDown},
                {InputType::StrafeRight, InputContext::Gameplay, KeyboardKey::D, InputButtonInteraction::IsDown},
                {InputType::Jump, InputContext::Gameplay, KeyboardKey::Space, InputButtonInteraction::IsDown},
                {InputType::Sprint, InputContext::Gameplay, KeyboardKey::LShift, InputButtonInteraction::IsDown},
                {InputType::LevitateUp, InputContext::Gameplay, KeyboardKey::R, InputButtonInteraction::IsDown},
                {InputType::LevitateDown, InputContext::Gameplay, KeyboardKey::F, InputButtonInteraction::IsDown},
                {InputType::RollLeft, InputContext::Gameplay, KeyboardKey::Q, InputButtonInteraction::IsDown},
                {InputType::RollRight, InputContext::Gameplay, KeyboardKey::E, InputButtonInteraction::IsDown},

                {InputType::InventorySlot1, InputContext::Gameplay, KeyboardKey::Num1, InputButtonInteraction::OnPressed},
                {InputType::InventorySlot2, InputContext::Gameplay, KeyboardKey::Num2, InputButtonInteraction::OnPressed},
                {InputType::InventorySlot3, InputContext::Gameplay, KeyboardKey::Num3, InputButtonInteraction::OnPressed},
                {InputType::InventorySlot4, InputContext::Gameplay, KeyboardKey::Num4, InputButtonInteraction::OnPressed},
                {InputType::InventorySlot5, InputContext::Gameplay, KeyboardKey::Num5, InputButtonInteraction::OnPressed},

                // TODO avoid this duplication.
                {InputType::InventorySlot1, InputContext::UI, KeyboardKey::Num1, InputButtonInteraction::OnPressed},
                {InputType::InventorySlot2, InputContext::UI, KeyboardKey::Num2, InputButtonInteraction::OnPressed},
                {InputType::InventorySlot3, InputContext::UI, KeyboardKey::Num3, InputButtonInteraction::OnPressed},
                {InputType::InventorySlot4, InputContext::UI, KeyboardKey::Num4, InputButtonInteraction::OnPressed},
                {InputType::InventorySlot5, InputContext::UI, KeyboardKey::Num5, InputButtonInteraction::OnPressed},
        };

const std::vector<InputMouseButtonMapping> DEFAULT_MOUSE_BUTTON_MAPPINGS
        {
                {InputType::Interact, InputContext::Gameplay, sf::Mouse::Left, InputButtonInteraction::OnReleased},
                {InputType::InteractAlternate, InputContext::Gameplay, sf::Mouse::Right, InputButtonInteraction::OnReleased},
                {InputType::MenuAccept, InputContext::UI, sf::Mouse::Left, InputButtonInteraction::OnReleased},
        };

Input::Input(std::shared_ptr<sf::Window> _window, const InputConfig& _config, bool _unlockMouse)
{
    m_Window = _window;

    // TODO Read from a config.
    m_KeyMappings = _config.m_KeyMappings;
    m_MouseButtonMappings = DEFAULT_MOUSE_BUTTON_MAPPINGS;
    m_IsMouseUnlocked = _unlockMouse;
    
    RecenterMouse();
}

void Input::UpdateUserInput(World* _world, UIDisplay* _display)
{
    if (_display == nullptr)
    {
        return;
    }

    const bool previousMouseInput = m_IsUsingMouseInput;

    sf::Event windowEvent;
    while (m_Window->pollEvent(windowEvent))
    {
        switch (windowEvent.type)
        {
            case sf::Event::Closed:
            {
                Engine::GetInstance().RequestQuit();
                break;
            }
            case sf::Event::KeyPressed:
            {
                HandleKeyInput(static_cast<KeyboardKey>(windowEvent.key.code), InputButtonInteraction::OnPressed, _world, _display);

                break;
            }
            case sf::Event::KeyReleased:
            {
                HandleKeyInput(static_cast<KeyboardKey>(windowEvent.key.code), InputButtonInteraction::OnReleased, _world, _display);

                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                HandleMouseButtonInput(windowEvent.mouseButton.button, InputButtonInteraction::OnReleased, _world, _display);

                break;
            }
            case sf::Event::TextEntered:
            {
                sf::String inputString = windowEvent.text.unicode;
                _display->OnTextEntered(inputString.toAnsiString());    // TODO Enforce UTF8 in all strings

                break;
            }
            default:
                break;
        }
    }

    if (!_display->IsInMenu() && _world != nullptr)
    {
        for (InputKeyMapping mapping : m_KeyMappings)
        {
            if (mapping.m_InputContext == InputContext::Gameplay
                && mapping.m_Interaction == InputButtonInteraction::IsDown
                && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(mapping.m_Key)))
            {
                _world->OnButtonInput(mapping.m_InputType);
            }
        }
    }

    const sf::Vector2i pos = sf::Mouse::getPosition(*m_Window);

    if (!_display->IsInMenu() && !m_IsMouseUnlocked && _world != nullptr)
    {
        const f32 pX = pos.x / static_cast<f32>(m_Window->getSize().x) - 0.5f;
        const f32 pY = pos.y / static_cast<f32>(m_Window->getSize().y) - 0.5f;

        _world->OnMouseInput(pX, pY);

        RecenterMouse();
    }
    else
    {
        if (pos != m_PreviousMousePosition)
        {
            m_IsUsingMouseInput = true;
            _display->OnMouseHover(pos.x, pos.y);
        }
    }

    if (previousMouseInput != m_IsUsingMouseInput)
    {
        _display->OnInputDeviceChanged(m_IsUsingMouseInput);
    }

    m_PreviousMousePosition = pos;

    
    if (_display->IsInMenu() || m_IsMouseUnlocked)
    {
        m_Window->setMouseCursorVisible(true);
        m_Window->setMouseCursorGrabbed(false);
    }
    else
    {
        m_Window->setMouseCursorVisible(false);
        m_Window->setMouseCursorGrabbed(true);
    }
}

void Input::HandleEvent(EngineEvent _event)
{
    switch (_event.GetType())
    {
    case EngineEvent::Type::InputSetMouseLock:
    {
        m_IsMouseUnlocked = (_event.GetIntData() != 1);
        break;
    }
    default:
        break;
    }
}

void Input::HandleKeyInput(KeyboardKey _key, InputButtonInteraction _interaction, World* _world, UIDisplay* _display)
{
    for (const InputKeyMapping& mapping : m_KeyMappings)
    {
        if (mapping.m_Key == _key && mapping.m_Interaction == _interaction)
        {
            if (mapping.m_InputContext == InputContext::UI && _display->IsInMenu())
            {
                m_IsUsingMouseInput = false;
            }

            HandleInput(mapping.m_InputType, mapping.m_InputContext, _world, _display);
        }
    }
}

void Input::HandleMouseButtonInput(sf::Mouse::Button _button, InputButtonInteraction _interaction, World* _world, UIDisplay* _display)
{
    for (const InputMouseButtonMapping& mapping : m_MouseButtonMappings)
    {
        if (mapping.m_Button == _button && mapping.m_Interaction == _interaction)
        {
            HandleInput(mapping.m_InputType, mapping.m_InputContext, _world, _display);
        }
    }
}

void Input::HandleInput(InputType _inputType, InputContext _context, World* _world, UIDisplay* _display)
{
    switch (_context)
    {
    case InputContext::Debug:
    {
        OnDebugInput(_inputType);

        break;
    }
    case InputContext::UI:
    {
        _display->OnButtonInput(_inputType);
        break;
    }
    case InputContext::Gameplay:
    {
        if(_display->IsInMenu())
        {
            break;
        }

        if (_world != nullptr)
        {
            _world->OnButtonInput(_inputType);
        }
        break;
    }
    default:
        break;
    }
}

void Input::OnDebugInput(InputType _inputType)
{
    if (_inputType == InputType::EngineQuit)
    {
        std::cout << "Escape pressed" << std::endl;
        Engine::GetInstance().RequestQuit();
    }
}

void Input::RecenterMouse()
{
    const sf::Vector2i mousePos = sf::Vector2i(m_Window->getSize() / static_cast<u32>(2));
    sf::Mouse::setPosition(mousePos, *m_Window);
}
