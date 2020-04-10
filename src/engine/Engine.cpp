//
// Created by alastair on 27/11/17.
//

#include "Engine.h"

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

    const std::optional<EngineConfig> loadedConfig = InspectionHelpers::LoadFromText<EngineConfig>("engineconfig.txt");

    if (loadedConfig.has_value())
    {
        m_EngineConfig = *loadedConfig;
    }
    else
    {
        const sf::VideoMode defaultMode = sf::VideoMode::getDesktopMode();
        m_EngineConfig.m_Resolution.x = defaultMode.width;
        m_EngineConfig.m_Resolution.y = defaultMode.height;
    }

    if (m_CommandLineArgs.m_ForceWindowed)
    {
        m_EngineConfig.m_Resolution.x = 1280;
        m_EngineConfig.m_Resolution.y = 720;
    }

    sf::ContextSettings settings;

    LogMessage("SFML OpenGL context: " + std::to_string(settings.majorVersion) + "." + std::to_string(settings.minorVersion));
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = m_EngineConfig.m_AntialiasingLevel;

    m_Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(m_EngineConfig.m_Resolution.x, m_EngineConfig.m_Resolution.y),
                                                  freeplanet_APP_NAME_PRETTY,
                                                  (m_EngineConfig.m_IsFullscreen && !m_CommandLineArgs.m_ForceWindowed ? sf::Style::Fullscreen : sf::Style::Close),
                                                  settings);

    if (m_Window->getSettings().minorVersion != settings.minorVersion || m_Window->getSettings().majorVersion != settings.majorVersion)
    {
        LogWarning("An OpenGL context was created that doesn't match what was requested: " + std::to_string(m_Window->getSettings().majorVersion)
                   + "." + std::to_string(m_Window->getSettings().minorVersion));
    }

    m_Window->setVerticalSyncEnabled(m_EngineConfig.m_IsVsyncEnabled);

    if (!m_CommandLineArgs.m_ForceUnlockedMouse)
    {
        m_Window->setMouseCursorVisible(false);
        m_Window->setMouseCursorGrabbed(true);
    }

    if (!loadedConfig.has_value())
    {
        // Do this after the window has been created to make sure we get the resolution that is actually being displayed.
        // Avoids issues where SFML is detecting multiple monitor resolutions incorrectly on Linux.
        LogMessage("No engineconfig.txt found, creating.");

        m_EngineConfig.m_Resolution.x = m_Window->getSize().x;
        m_EngineConfig.m_Resolution.y = m_Window->getSize().y;

        InspectionHelpers::SaveToText(m_EngineConfig, "engineconfig.txt");
    }
    
    m_Window->setActive(false);

    m_RenderHandler = std::make_shared<RenderHandler>(m_Window);

    m_Input = new Input(m_Window);
    m_UIDisplay = std::make_shared<UIDisplay>(m_Window.get());
    m_UIDisplay->UpdateSplashScreen();

    m_RenderHandler->Render(nullptr, nullptr, m_UIDisplay);

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

    if (m_CommandLineArgs.m_Noclip)
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
        }

        if (m_IsCameraChangePending)
        {
            ChangeCamera();
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

const FreelookCameraComponent* Engine::GetActiveCamera()
{
    if (m_World == nullptr)
    {
        return nullptr;
    }

    if (m_ActiveCameraObject == WORLDOBJECTID_INVALID)
    {
        return nullptr;
    }

    const WorldObject* cameraObject = m_World->GetWorldObject(m_ActiveCameraObject);
    if (cameraObject == nullptr)
    {
        LogError("Couldn't find the active camera in the world.");
        return nullptr;
    }

    return cameraObject->GetFreelookCameraComponent();
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

    const FreelookCameraComponent* cameraComponent = GetActiveCamera();

    m_RenderHandler->Render(m_World.get(), cameraComponent, m_UIDisplay);

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
        break;
    }
    case EngineEvent::Type::EngineLoadPlanetFromSeed:
    {
        m_RequestedWorld = std::make_shared<World>("Generated World", PlanetGeneration::GenerateFromSeed(_event.GetIntData()));
        break;
    }
    case EngineEvent::Type::OnQuit:
    {
        LogMessage("OnQuit");
        m_IsGameplayRunning = false;
        m_World = nullptr;
        m_RequestedWorld = nullptr;
        break;
    }
    default:
        break;
    }
}

void Engine::RequestCameraChange(WorldObjectID _cameraObjectID)
{
    m_RequestedCameraObject = _cameraObjectID;
    m_IsCameraChangePending = true;
}

void Engine::OnCameraDestroyed(WorldObjectID _cameraObjectID)
{
    if (m_ActiveCameraObject == _cameraObjectID)
    {
        m_ActiveCameraObject = WORLDOBJECTID_INVALID;
    }
    if (m_RequestedCameraObject == _cameraObjectID)
    {
        m_RequestedCameraObject = WORLDOBJECTID_INVALID;
    }
}

void Engine::ChangeCamera()
{
    if (!m_IsGameplayRunning || !m_World)
    {
        LogError("Tried to set camera when no world gameplay is running.");
        return;
    }

    LogMessage("Changing camera...");

    m_ActiveCameraObject = m_RequestedCameraObject;
    m_IsCameraChangePending = false;
}

bool Engine::IsInMenu() const
{
    // TODO: I don't really think this should be an engine method. I'd like
    // a nicer way to handle this
    if (m_UIDisplay == nullptr)
    {
        LogWarning("Tried to check for menu when there is no UI");
        return false;
    }

    return m_UIDisplay->IsInMenu();
}

const CommandLineArgs& Engine::GetCommandLineArgs() const
{
    return m_CommandLineArgs;
}
