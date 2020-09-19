//
// Created by alastair on 07/10/17.
//

#include "UIDisplay.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <src/engine/Engine.h>
#include <src/graphics/ui/UIActions.h>
#include <src/graphics/ui/UIConsoleMenu.h>
#include <src/graphics/ui/debug/UIAspectRatioDebug.h>
#include <src/graphics/ui/debug/UICollisionDebug.h>
#include <src/graphics/ui/debug/UIComponentDebug.h>
#include <src/graphics/ui/debug/UIWorldZoneDebug.h>
#include <src/graphics/ui/debug/UITerrainDebug.h>
#include <src/graphics/ui/debug/UIProfilingDebug.h>
#include <src/graphics/ui/debug/UIPlanetDebug.h>
#include <src/graphics/ui/menus/UIMainMenu.h>
#include <src/profiling/Profiler.h>
#include <src/world/World.h>
#include <src/world/WorldZone.h>
#include <src/graphics/ui/hud/UIHUDMenu.h>

void UIDisplay::Update(TimeMS delta, const World* _world)
{
    ProfileCurrentFunction();

    m_DrawingQueue.clear();
    m_Debug3DDrawingQueue.clear();

    if (m_RequestedGameMenu != nullptr)
    {
        m_GameMenu = std::move(m_RequestedGameMenu);

        // When we open a new menu, if using a keyboard/controller, the first item should already be focused.
        // If using a mouse, focus should depend on cursor movement.
        const bool shouldStartFocused = !m_IsUsingMouseInput;
        m_GameMenu->Init(shouldStartFocused);

        m_RequestedGameMenu = nullptr;
    }

    if (!m_RequestedDebugMenu.empty())
    {
        if (m_RequestedDebugMenu == "components")
        {
            auto debugMenu = std::make_unique<UIComponentDebug>();
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "zones")
        {
            auto debugMenu = std::make_unique<UIWorldZoneDebug>();
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "terrain")
        {
            auto debugMenu = std::make_unique<UITerrainDebug>();
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "profile")
        {
            auto debugMenu = std::make_unique<UIProfilingDebug>();
            debugMenu->InitProfiler(&Profiler::GetInstance());
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "collision")
        {
            auto debugMenu = std::make_unique<UICollisionDebug>();
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "planet")
        {
            auto debugMenu = std::make_unique<UIPlanetDebug>();
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "aspect")
        {
            auto debugMenu = std::make_unique<UIAspectRatioDebug>();
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "none" || m_RequestedDebugMenu == "off")
        {
            m_DebugMenu.reset();
        }
        else
        {
            LogError(m_RequestedDebugMenu + " is not a valid debug menu.");
        }

        m_RequestedDebugMenu = "";
    }

    if (m_GameMenu != nullptr)
    {
        m_GameMenu->Draw(delta, m_Interface, _world);
    }

    if (m_DisplayLog && m_Console != nullptr)
    {
        m_Console->Draw(delta, m_Interface, _world);
    }
    else
    {
        m_Interface.DrawRectangle(glm::vec2(0,0), glm::vec2(1920,1080), Color(0,0,0,0));

        if (m_DebugMenu != nullptr)
        {
            m_DebugMenu->Draw(delta, m_Interface, _world);
        }
    }

    if (m_DisplayLog || (m_GameMenu != nullptr && m_GameMenu->ShouldTakeFocus()) || Engine::GetInstance().GetCommandLineArgs().m_ForceUnlockedMouse)
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

UIDisplay::UIDisplay(sf::RenderWindow *_window)
{
    m_Window = _window;

    if (!m_UIFont.loadFromFile(Globals::FREEPLANET_ASSET_PATH + "fonts/Roboto-Regular.ttf"))
    {
        LogError("UI font load failed");
    }

    if (!m_MonospaceFont.loadFromFile(Globals::FREEPLANET_ASSET_PATH + "fonts/RobotoMono-Regular.ttf"))
    {
        LogMessage("Monospace font load failed");
    }

    m_Interface.SetUIDisplay(this);
    m_Actions.SetUIDisplay(this);
    m_Console = std::make_unique<UIConsoleMenu>();
    m_RequestedGameMenu = std::make_unique<UIMainMenu>();
}

void UIDisplay::OnButtonInput(InputType _type)
{
    if (_type == InputType::ToggleLog)
    {
        m_DisplayLog = !m_DisplayLog;
    }

    if (m_GameMenu != nullptr)
    {
        m_GameMenu->OnButtonReleased(_type, m_Actions);
    }

    if (m_DebugMenu != nullptr)
    {
        m_DebugMenu->OnButtonReleased(_type, m_Actions);
    }

    if (m_Console != nullptr)
    {
        m_Console->OnButtonReleased(_type, m_Actions);
    }
}

void UIDisplay::OnTextEntered(std::string _value)
{
    if(!IsInMenu())
    {
        return;
    }

    if (m_GameMenu != nullptr && !m_DisplayLog)
    {
        m_GameMenu->OnTextEntered(_value);
    }

    if (m_DebugMenu != nullptr)
    {
        m_DebugMenu->OnTextEntered(_value);
    }

    if (m_Console != nullptr && m_DisplayLog)
    {
        m_Console->OnTextEntered(_value);
    }
}

void UIDisplay::OnMouseHover(f32 _x, f32 _y)
{
    if (m_DebugMenu != nullptr)
    {
        m_DebugMenu->OnMouseHover(m_Interface, _x, _y);
    }

    if (m_GameMenu != nullptr)
    {
        m_GameMenu->OnMouseHover(m_Interface, _x, _y);
    }
}

bool UIDisplay::IsInMenu()
{
    return m_DisplayLog || (m_GameMenu != nullptr && m_GameMenu->ShouldTakeFocus());
}

void UIDisplay::UpdateSplashScreen()
{
    m_Interface.DrawRectangle(glm::vec2(0), glm::ivec2(1920,1080), Color(0.5f, 0.5f, 0.5f, 1.f));
    m_Interface.DrawString(glm::ivec2(100, 100), freeplanet_APP_NAME_PRETTY, 64.f, Color(1.f));
    m_Interface.DrawString(glm::ivec2(100, 948), "Reticulating splines...", 32.f, Color(1.f));
}

void UIDisplay::AddEvent(EngineEvent _event)
{
    Engine::GetInstance().AddEvent(_event);
}

void UIDisplay::HandleEvent(EngineEvent _event)
{
    if (_event.GetType() == EngineEvent::Type::UIRequestDebugMenu)
    {
        m_RequestedDebugMenu = _event.GetStringData();
        m_DisplayLog = false;
    }
}

void UIDisplay::RequestQuit()
{
    Engine::GetInstance().RequestQuit();
}

void UIDisplay::OpenHUD()
{
    RequestMenu<UIHUDMenu>();
}

void UIDisplay::CloseGameMenu()
{
    m_GameMenu.release();
}
