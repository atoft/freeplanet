//
// Created by alastair on 27/11/17.
//

#pragma once

#include <src/engine/EngineConfig.h>
#include <src/engine/events/EventHandler.h>
#include <src/engine/StandardInputTask.h>
#include <src/engine/StartupHelpers.h>
#include <src/world/WorldObjectID.h>

class FreelookCameraComponent;
class Input;
class RenderHandler;
class UIDisplay;
class World;

namespace sf
{
    class RenderWindow;
    class Window;
}

class Engine
{
public:
    Engine(Engine const&) = delete;
    void operator=(Engine const&) = delete;

    static Engine& GetInstance();

    s32 Run(const CommandLineArgs& _commandLineArgs);

    void RequestCameraChange(WorldObjectID _cameraObjectID);
    void OnCameraDestroyed(WorldObjectID _cameraObjectID);
    void RequestQuit();
    void AddEvent(EngineEvent _event);

    const CommandLineArgs& GetCommandLineArgs() const;

    bool IsInMenu() const;

private:
    Engine() = default;

    void RunFrame(TimeMS delta);
    void HandleEvents(TimeMS _delta);
    void HandleEvent(EngineEvent _event);

    void ChangeCamera();
    const FreelookCameraComponent* GetActiveCamera();

private:
    CommandLineArgs m_CommandLineArgs;
    EngineConfig m_EngineConfig;

    std::shared_ptr<sf::RenderWindow> m_Window = nullptr;

    std::shared_ptr<World> m_World = nullptr;
    std::shared_ptr<World> m_RequestedWorld = nullptr;

    std::shared_ptr<RenderHandler> m_RenderHandler = nullptr;
    std::shared_ptr<UIDisplay> m_UIDisplay = nullptr;

    EventHandler<EngineEvent> m_EventHandler;

    u64 m_FrameCount = 0;

    WorldObjectID m_ActiveCameraObject = WORLDOBJECTID_INVALID;
    WorldObjectID m_RequestedCameraObject = WORLDOBJECTID_INVALID;
    bool m_IsCameraChangePending = false;

    Input* m_Input = nullptr;

    bool m_IsGameplayRunning = false;

    StandardInputTask m_StandardInputTask;
};
