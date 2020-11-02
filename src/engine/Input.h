//
// Created by alastair on 23/07/17.
//

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
