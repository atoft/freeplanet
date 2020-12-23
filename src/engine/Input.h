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

#include <memory>
#include <vector>

#include <src/engine/InputTypes.h>
#include <src/engine/events/EngineEvent.h>

namespace sf
{
    class Window;
}
struct InputConfig;
class World;
class UIDisplay;
class WorldObject;

// Handles the passing of user input to registered WorldObjects.
class Input
{
public:
    void UpdateUserInput(World *_world, UIDisplay *_display);
    void HandleEvent(EngineEvent _event);

    Input(std::shared_ptr<sf::Window> _window, const InputConfig& _config, bool _unlockMouse);

private:
    void HandleKeyInput(KeyboardKey _key, InputButtonInteraction _interaction, World* _world, UIDisplay* _display);
    void HandleMouseButtonInput(sf::Mouse::Button _button, InputButtonInteraction _interaction, World* _world, UIDisplay* _display);

    void HandleInput(InputType _inputType, InputContext _context, World* _world, UIDisplay* _display);

    void OnDebugInput(InputType _inputType);

    void RecenterMouse();

public:
    static std::vector<InputKeyMapping> DEFAULT_KEY_MAPPINGS;

private:
    std::shared_ptr<sf::Window> m_Window;

    std::vector<InputKeyMapping> m_KeyMappings;
    std::vector<InputMouseButtonMapping> m_MouseButtonMappings;

    sf::Vector2i m_PreviousMousePosition = sf::Vector2i(0, 0);
    bool m_IsUsingMouseInput = false;

    bool m_IsMouseUnlocked = false;
};
