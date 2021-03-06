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

#include "Engine.h"

#include <filesystem>

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <src/config.h>
#include <src/engine/Input.h>
#include <src/engine/inspection/InspectionHelpers.h>
#include <src/graphics/ui/UIDisplay.h>
#include <src/graphics/RenderHandler.h>
#include <src/profiling/Profiler.h>
#include <src/tools/TestWorld.h>
#include <src/world/World.h>
#include <src/world/WorldObject.h>
#include <src/world/planet/PlanetGeneration.h>
#include <src/world/ComponentAccess.h>

Engine& Engine::GetInstance()
{
    static Engine instance;
    return instance;
}

s32 Engine::Run(const CommandLineArgs& _commandLineArgs)
{
    LogMessage("Starting " + std::string(freeplanet_APP_NAME_PRETTY)
               + "\n\tVersion " + std::to_string(freeplanet_VERSION_MAJOR) + "." + std::to_string(freeplanet_VERSION_MINOR));

    ThreadUtils::tl_ThreadType = ThreadType::Main;
    m_CommandLineArgs = _commandLineArgs;

    // Make a directory for savegames.
    std::filesystem::create_directory("saved");

    const InspectionHelpers::LoadFromTextResult<EngineConfig> loadedConfig = InspectionHelpers::LoadFromTextFile<EngineConfig>("engineconfig.txt");

    // If it couldn't be loaded, or it was missing some values, we want to update the file on disk.
    const bool needToWriteBack = loadedConfig.m_Result == InspectionResult::ReadIncomplete;
    const bool needToCreate = loadedConfig.m_Result == InspectionResult::ReadSyntaxError || loadedConfig.m_Result == InspectionResult::FileIOError;

    if (loadedConfig.m_Result == InspectionResult::Success || loadedConfig.m_Result == InspectionResult::ReadIncomplete)
    {
        m_EngineConfig = *loadedConfig.m_Value;
    }
    else
    {
        const sf::VideoMode defaultMode = sf::VideoMode::getDesktopMode();
        m_EngineConfig.m_GraphicsConfig.m_Resolution.x = defaultMode.width;
        m_EngineConfig.m_GraphicsConfig.m_Resolution.y = defaultMode.height;
    }

    if (m_CommandLineArgs.m_ForceWindowed)
    {
        m_EngineConfig.m_GraphicsConfig.m_Resolution.x = 1280;
        m_EngineConfig.m_GraphicsConfig.m_Resolution.y = 720;
    }

    sf::ContextSettings settings;

    LogMessage("SFML OpenGL context: " + std::to_string(settings.majorVersion) + "." + std::to_string(settings.minorVersion));
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = m_EngineConfig.m_GraphicsConfig.m_AntialiasingLevel;

    m_Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(m_EngineConfig.m_GraphicsConfig.m_Resolution.x, m_EngineConfig.m_GraphicsConfig.m_Resolution.y),
                                                  freeplanet_APP_NAME_PRETTY,
                                                  (m_EngineConfig.m_GraphicsConfig.m_IsFullscreen && !m_CommandLineArgs.m_ForceWindowed ? sf::Style::Fullscreen : sf::Style::Close),
                                                  settings);

    if (m_Window->getSettings().minorVersion != settings.minorVersion || m_Window->getSettings().majorVersion != settings.majorVersion)
    {
        LogWarning("An OpenGL context was created that doesn't match what was requested: " + std::to_string(m_Window->getSettings().majorVersion)
                   + "." + std::to_string(m_Window->getSettings().minorVersion));
    }

    m_Window->setVerticalSyncEnabled(m_EngineConfig.m_GraphicsConfig.m_IsVsyncEnabled);

    if (!m_CommandLineArgs.m_ForceUnlockedMouse)
    {
        m_Window->setMouseCursorVisible(false);
        m_Window->setMouseCursorGrabbed(true);
    }

    const std::string engineConfigFilename = "engineconfig.txt";

    if (needToCreate)
    {
        // Do this after the window has been created to make sure we get the resolution that is actually being displayed.
        // Avoids issues where SFML is detecting multiple monitor resolutions incorrectly on Linux.
        LogMessage("No " + engineConfigFilename + " found, creating.");

        m_EngineConfig.m_GraphicsConfig.m_Resolution.x = m_Window->getSize().x;
        m_EngineConfig.m_GraphicsConfig.m_Resolution.y = m_Window->getSize().y;

        m_EngineConfig.m_InputConfig.m_KeyMappings = Input::DEFAULT_KEY_MAPPINGS;

        InspectionHelpers::SaveToTextFile(m_EngineConfig, engineConfigFilename);
    }
    else if (needToWriteBack)
    {
        LogMessage("Updating " + engineConfigFilename + ".");
        InspectionHelpers::SaveToTextFile(m_EngineConfig, engineConfigFilename);
    }
    
    m_Window->setActive(false);

    m_RenderHandler = std::make_shared<RenderHandler>(m_Window, m_EngineConfig.m_GraphicsConfig);

    m_Input = std::make_shared<Input>(m_Window, m_EngineConfig.m_InputConfig, m_CommandLineArgs.m_ForceUnlockedMouse);
    m_UIDisplay = std::make_shared<UIDisplay>(m_Window.get());
    m_UIDisplay->UpdateSplashScreen();

    m_RenderHandler->Render(nullptr, m_UIDisplay);

    if (m_CommandLineArgs.m_UseStandardInput)
    {
        m_StandardInputTask.Start();

    }

    if (!m_CommandLineArgs.m_TestWorld.empty())
    {
        m_RequestedWorld = Test::BuildTestWorld(m_CommandLineArgs.m_TestWorld);

        if (m_RequestedWorld != nullptr)
        {
            m_RequestedWorld->HandleCommandLineArgs(m_CommandLineArgs);
        }
    }

    if (!m_CommandLineArgs.m_DebugUI.empty())
    {
        const EngineEvent event = EngineEvent(EngineEvent::Type::UIRequestDebugMenu, m_CommandLineArgs.m_DebugUI);
        AddEvent(event);
    }

    sf::Clock clock;
    m_IsGameplayRunning = true;
    while (m_IsGameplayRunning) 
    {
        if (m_RequestedWorld != m_World)
        {
            LogMessage("Changing world from "
                + (m_World != nullptr ? m_World->GetName() : "[none]")
                + " to "
                + (m_RequestedWorld != nullptr? m_RequestedWorld->GetName() : "[none]"));

            m_World = m_RequestedWorld;

            m_World->RegisterLocalPlayer(0);
            m_World->HandleCommandLineArgs(m_CommandLineArgs);
        }

        TimeMS delta = clock.getElapsedTime().asSeconds() * 1000.f;
        clock.restart();

        Profiler::GetInstance().Update(delta);

        constexpr TimeMS FRAME_DELTA_CAP = 100.f;
        constexpr TimeMS FRAME_DELTA_TARGET = 1000.f / 60.f;

        if (delta > FRAME_DELTA_CAP)
        {
            LogWarning("A very long frame was detected (duration " + std::to_string(delta) + " ms). "
                "The delta will be clamped for the following frame.");
            delta = FRAME_DELTA_TARGET;
        }
        if (delta < 0.f)
        {
            LogError("Invalid delta time detected! (duration " + std::to_string(delta) + " ms). ");
            assert(false);
        }

        RunFrame(delta);
    }

    return 0;
}

void Engine::RequestQuit()
{
    AddEvent(EngineEvent(EngineEvent::Type::EngineRequestQuit));
}

void Engine::AddEvent(EngineEvent _event)
{
    m_EventHandler.PushEvent(_event);
}

void Engine::RunFrame(TimeMS delta)
{
    // TODO Input to the World should go via Players
    m_Input->UpdateUserInput(m_World.get(), m_UIDisplay.get());

    if (m_World != nullptr)
    {
        m_World->Update(delta);
    }

    if (m_UIDisplay != nullptr)
    {
        m_UIDisplay->Update(delta, m_World.get());
    }

    HandleEvents(delta);

    m_RenderHandler->Render(m_World.get(), m_UIDisplay);

    ++m_FrameCount;
}

void Engine::HandleEvents(TimeMS _delta)
{
    ProfileCurrentFunction();

    std::vector<EngineEvent> allEvents = m_EventHandler.PopEvents();

    for (const EngineEvent& event : allEvents)
    {
        HandleEvent(event);
        m_StandardInputTask.HandleEvent(event);
        m_RenderHandler->HandleEvent(event);
        m_Input->HandleEvent(event);

        if (m_UIDisplay != nullptr)
        {
            m_UIDisplay->HandleEvent(event);
        }

        if (m_World != nullptr)
        {
            m_World->HandleEvent(event);
        }
    }
}

void Engine::HandleEvent(EngineEvent _event)
{
    switch (_event.GetType())
    {
    case EngineEvent::Type::Echo:
    {
        LogMessage("Echo: " + _event.GetStringData());
        break;
    }
    case EngineEvent::Type::Help:
    {
        LogMessage(EngineEvent::GetHelpText());
        break;
    }
    case EngineEvent::Type::EngineRequestQuit:
    {
        AddEvent(EngineEvent(EngineEvent::Type::OnQuit));
        break;
    }
    case EngineEvent::Type::EngineTestWorld:
    {
        m_RequestedWorld = Test::BuildTestWorld(_event.GetStringData());
        m_UIDisplay->OpenHUD();
        break;
    }
    case EngineEvent::Type::EngineNewPlanet:
    {
        m_RequestedWorld = std::make_shared<World>();

        const EnginePlanetRequest request = _event.Get<EnginePlanetRequest>();

        const bool newWorldSuccess = m_RequestedWorld->InitializeNew(request.m_Name, request.m_Seed);

        if (!newWorldSuccess)
        {
            LogError("Failed to create new world.");
        }

        m_UIDisplay->OpenHUD();
        break;
    }
    case EngineEvent::Type::EngineLoadPlanet:
    {
        m_RequestedWorld = std::make_shared<World>();

        const bool newWorldSuccess = m_RequestedWorld->LoadFromFile(_event.GetStringData());

        if (!newWorldSuccess)
        {
            LogError("Failed to create new world.");
        }

        m_UIDisplay->OpenHUD();
        break;
    }
    case EngineEvent::Type::OnQuit:
    {
        LogMessage("OnQuit");
        m_IsGameplayRunning = false;
        m_RequestedWorld = nullptr;
        break;
    }
    default:
        break;
    }
}

const CommandLineArgs& Engine::GetCommandLineArgs() const
{
    return m_CommandLineArgs;
}
