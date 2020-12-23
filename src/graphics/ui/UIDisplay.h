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

#include <list>
#include <variant>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <src/engine/InputTypes.h>
#include <src/engine/events/EngineEvent.h>
#include <src/graphics/Scene.h>
#include <src/graphics/ui/UIBaseMenu.h>
#include <src/graphics/ui/UIActions.h>
#include <src/graphics/ui/UIDrawInterface.h>

namespace sf
{
    class RenderWindow;
}

class UIDrawInterface;
class UIBaseMenu;
class World;

using ZoneDebugRenderable = std::pair<glm::ivec3, Renderable::SceneObject>;

class UIDisplay
{
    friend class UIDrawInterface;
    friend class RenderHandler;

public:
    explicit UIDisplay(sf::RenderWindow* _window);

    void Update(TimeMS delta, const World *_world);

    void OnButtonInput(InputType _type);
    void OnTextEntered(std::string _value);
    void OnMouseHover(f32 _x, f32 _y);
    void OnInputDeviceChanged(bool _isUsingMouseInput) { m_IsUsingMouseInput = _isUsingMouseInput; };

    void UpdateSplashScreen();

    bool IsInMenu();

    void AddEvent(EngineEvent _event);
    void HandleEvent(EngineEvent _event);

    void RequestQuit();

    void OpenHUD();
    void CloseGameMenu();

public:
    template <typename T>
    void RequestMenu()
    {
        m_RequestedGameMenu = std::make_unique<T>();
    }

private:
    static constexpr u32 MAX_COLOR = 255;

    sf::RenderWindow* m_Window;
    sf::Font m_UIFont;
    sf::Font m_MonospaceFont;

    bool m_DisplayLog = false;

    UIDrawInterface m_Interface;
    UIActions m_Actions;
    bool m_IsUsingMouseInput = false;

    std::unique_ptr<UIBaseMenu> m_GameMenu;
    std::unique_ptr<UIBaseMenu> m_DebugMenu;
    std::unique_ptr<UIBaseMenu> m_Console;

    std::unique_ptr<UIBaseMenu> m_RequestedGameMenu;
    std::string m_RequestedDebugMenu;

    std::vector<Renderable::DrawableVariant> m_DrawingQueue;
    std::vector<ZoneDebugRenderable> m_Debug3DDrawingQueue;
};
