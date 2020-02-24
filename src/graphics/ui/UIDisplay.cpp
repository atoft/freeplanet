//
// Created by alastair on 07/10/17.
//

#include "UIDisplay.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <src/engine/Engine.h>
#include <src/graphics/ui/UIConsoleMenu.h>
#include <src/graphics/ui/UICollisionDebug.h>
#include <src/graphics/ui/UIComponentDebug.h>
#include <src/graphics/ui/UIWorldZoneDebug.h>
#include <src/graphics/ui/UITerrainDebug.h>
#include <src/graphics/ui/UIProfilingDebug.h>
#include <src/profiling/Profiler.h>
#include <src/world/World.h>
#include <src/world/WorldZone.h>

void UIDisplay::Update(TimeMS delta, const World* _world)
{
    ProfileCurrentFunction();

    m_DrawingQueue.clear();
    m_Debug3DDrawingQueue.clear();

    if (!m_RequestedDebugMenu.empty())
    {
        if (m_RequestedDebugMenu == "components")
        {
            auto debugMenu = std::make_unique<UIComponentDebug>();
            debugMenu->Init(_world);
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "zones")
        {
            auto debugMenu = std::make_unique<UIWorldZoneDebug>();
            debugMenu->Init(_world);
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "terrain")
        {
            auto debugMenu = std::make_unique<UITerrainDebug>();
            debugMenu->Init(_world);
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "profile")
        {
            auto debugMenu = std::make_unique<UIProfilingDebug>();
            debugMenu->Init(&Profiler::GetInstance());
            m_DebugMenu = std::move(debugMenu);
        }
        else if (m_RequestedDebugMenu == "collision")
        {
            auto debugMenu = std::make_unique<UICollisionDebug>();
            debugMenu->Init(_world);
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

    if (m_DisplayLog)
    {
        m_Window->setMouseCursorVisible(true);
        m_Window->setMouseCursorGrabbed(false);

        if (m_Console != nullptr)
        {
            m_Console->Draw(delta, m_Interface);
        }

    }
    else
    {
        m_Interface.DrawRectangle(glm::vec2(0,0), glm::vec2(1920,1080), Color(0,0,0,0));

        if (_world == nullptr)
        {
            m_Interface.DrawString(glm::vec2(20, 20), "No world is loaded. Use the console (tilde or F1) to enter a command.", 24.f);
        }

        if (m_DebugMenu != nullptr)
        {
            m_DebugMenu->Draw(delta, m_Interface);
        }

        if (!Engine::GetInstance().GetCommandLineArgs().m_ForceUnlockedMouse)
        {
            m_Window->setMouseCursorVisible(false);
            m_Window->setMouseCursorGrabbed(true);
        }
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
    m_Console = std::make_unique<UIConsoleMenu>();
}

void UIDisplay::OnButtonInput(InputType _type)
{
    if (_type == InputType::ToggleLog)
    {
        m_DisplayLog = !m_DisplayLog;
    }

    if (m_GameMenu != nullptr)
    {
        m_GameMenu->OnButtonReleased(_type);
    }

    if (m_DebugMenu != nullptr)
    {
        m_DebugMenu->OnButtonReleased(_type);
    }

    if (m_Console != nullptr)
    {
        m_Console->OnButtonReleased(_type);
    }
}

void UIDisplay::OnTextEntered(std::string _value)
{
    if(!IsInMenu())
    {
        return;
    }

    if (m_GameMenu != nullptr)
    {
        m_GameMenu->OnTextEntered(_value);
    }

    if (m_DebugMenu != nullptr)
    {
        m_DebugMenu->OnTextEntered(_value);
    }

    if (m_Console != nullptr)
    {
        m_Console->OnTextEntered(_value);
    }
}

bool UIDisplay::IsInMenu()
{
    return m_DisplayLog;
}

void UIDisplay::UpdateSplashScreen()
{
    m_Interface.DrawRectangle(glm::vec2(0), glm::ivec2(1920,1080), Color(0.5f, 0.5f, 0.5f, 1.f));
    m_Interface.DrawString(glm::ivec2(100, 100), freeplanet_APP_NAME_PRETTY, 64.f, Color(1.f));
    m_Interface.DrawString(glm::ivec2(100, 948), "Reticulating splines...", 32.f, Color(1.f));
}

void UIDisplay::HandleEvent(EngineEvent _event)
{
    if (_event.GetType() == EngineEvent::Type::UIRequestDebugMenu)
    {
        m_RequestedDebugMenu = _event.GetStringData();
        m_DisplayLog = false;
    }
}
