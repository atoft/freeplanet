//
// Created by alastair on 23/07/17.
//

#include "Input.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window.hpp>

#include <src/engine/Engine.h>
#include <src/graphics/ui/UIDisplay.h>
#include <src/world/World.h>

const std::vector<InputKeyMapping> DEFAULT_KEY_MAPPINGS
        {
                {InputType::EngineQuit, InputContext::Debug, sf::Keyboard::Escape, InputButtonInteraction::OnReleased},
                {InputType::MenuLeft, InputContext::UI, sf::Keyboard::Left, InputButtonInteraction::OnPressed},
                {InputType::MenuRight, InputContext::UI, sf::Keyboard::Right, InputButtonInteraction::OnPressed},
                {InputType::MenuUp, InputContext::UI, sf::Keyboard::Up, InputButtonInteraction::OnPressed},
                {InputType::MenuDown, InputContext::UI, sf::Keyboard::Down, InputButtonInteraction::OnPressed},

                {InputType::MenuAccept, InputContext::UI, sf::Keyboard::Return, InputButtonInteraction::OnPressed},
                {InputType::ToggleLog, InputContext::UI, sf::Keyboard::Tilde, InputButtonInteraction::OnPressed},

				// @CrossPlatform Grave/"UK tilde" is not recognised on Windows - at least with a UK Keyboard layout.
				// I think it's a bug or limitation in SFML and works by accident (?) on Linux.
				// Use F1 on Windows as a workaround.
				{InputType::ToggleLog, InputContext::UI, sf::Keyboard::F1, InputButtonInteraction::OnPressed},

                {InputType::MoveForward, InputContext::Gameplay, sf::Keyboard::W, InputButtonInteraction::IsDown},
                {InputType::MoveBack, InputContext::Gameplay, sf::Keyboard::S, InputButtonInteraction::IsDown},
                {InputType::StrafeLeft, InputContext::Gameplay, sf::Keyboard::A, InputButtonInteraction::IsDown},
                {InputType::StrafeRight, InputContext::Gameplay, sf::Keyboard::D, InputButtonInteraction::IsDown},
                {InputType::Jump, InputContext::Gameplay, sf::Keyboard::Space, InputButtonInteraction::IsDown},
                {InputType::Sprint, InputContext::Gameplay, sf::Keyboard::LShift, InputButtonInteraction::IsDown},
                {InputType::LevitateUp, InputContext::Gameplay, sf::Keyboard::R, InputButtonInteraction::IsDown},
                {InputType::LevitateDown, InputContext::Gameplay, sf::Keyboard::F, InputButtonInteraction::IsDown},
                {InputType::RollLeft, InputContext::Gameplay, sf::Keyboard::Q, InputButtonInteraction::IsDown},
                {InputType::RollRight, InputContext::Gameplay, sf::Keyboard::E, InputButtonInteraction::IsDown},
        };

const std::vector<InputMouseButtonMapping> DEFAULT_MOUSE_BUTTON_MAPPINGS
        {
                {InputType::Interact, InputContext::Gameplay, sf::Mouse::Left, InputButtonInteraction::OnReleased},
                {InputType::InteractAlternate, InputContext::Gameplay, sf::Mouse::Right, InputButtonInteraction::OnReleased},
        };

void Input::UpdateUserInput(World* _world, UIDisplay* _display)
{
    if (_display == nullptr)
    {
        return;
    }

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
                HandleKeyInput(windowEvent.key.code, InputButtonInteraction::OnPressed, _world, _display);

                break;
            }
            case sf::Event::KeyReleased:
            {
                HandleKeyInput(windowEvent.key.code, InputButtonInteraction::OnReleased, _world, _display);

                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                if(Engine::GetInstance().IsInMenu())
                {
                    break;
                }

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

    if (!Engine::GetInstance().IsInMenu() && _world != nullptr)
    {
        for (InputKeyMapping mapping : m_KeyMappings)
        {
            if (mapping.m_InputContext == InputContext::Gameplay
                && mapping.m_Interaction == InputButtonInteraction::IsDown
                && sf::Keyboard::isKeyPressed(mapping.m_Key))
            {
                _world->OnButtonInput(mapping.m_InputType);
            }
        }
    }

    const sf::Vector2i pos = sf::Mouse::getPosition(*m_Window);

    const f32 pX = pos.x / static_cast<f32>(Globals::FREEPLANET_WINDOW_WIDTH) - 0.5f;
    const f32 pY = pos.y / static_cast<f32>(Globals::FREEPLANET_WINDOW_HEIGHT) - 0.5f;

    const Engine& engine = Engine::GetInstance();
    if (!engine.IsInMenu() && !engine.GetCommandLineArgs().m_ForceUnlockedMouse && _world != nullptr)
    {
        _world->OnMouseInput(pX, pY);

        sf::Mouse::setPosition(sf::Vector2i(Globals::FREEPLANET_WINDOW_WIDTH/2 , Globals::FREEPLANET_WINDOW_HEIGHT/2), *m_Window);
    }
}

void Input::HandleKeyInput(sf::Keyboard::Key _key, InputButtonInteraction _interaction, World* _world, UIDisplay* _display)
{
    const auto& it = std::find_if(m_KeyMappings.begin(), m_KeyMappings.end(),
                                  [_key, _interaction](const InputKeyMapping& _mapping)
                                  {
                                      return _mapping.m_Key  == _key
                                         && _mapping.m_Interaction == _interaction;
                                  });

    if (it != m_KeyMappings.end())
    {
        HandleInput(it->m_InputType, it->m_InputContext, _world, _display);
    }
}

void Input::HandleMouseButtonInput(sf::Mouse::Button _button, InputButtonInteraction _interaction, World* _world, UIDisplay* _display)
{
    const auto& it = std::find_if(m_MouseButtonMappings.begin(), m_MouseButtonMappings.end(),
                                  [_button, _interaction](const InputMouseButtonMapping& _mapping)
                                  {
                                      return _mapping.m_Button  == _button
                                         && _mapping.m_Interaction == _interaction;
                                  });

    if (it != m_MouseButtonMappings.end())
    {
        HandleInput(it->m_InputType, it->m_InputContext, _world, _display);
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

Input::Input(std::shared_ptr<sf::Window> _window)
{
    m_Window = _window;

    // TODO Read from a config.
    m_KeyMappings = DEFAULT_KEY_MAPPINGS;
    m_MouseButtonMappings = DEFAULT_MOUSE_BUTTON_MAPPINGS;
}
