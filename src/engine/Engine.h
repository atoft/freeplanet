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

    void RequestQuit();
    void AddEvent(EngineEvent _event);

    const CommandLineArgs& GetCommandLineArgs() const;

private:
    Engine() = default;

    void RunFrame(TimeMS delta);
    void HandleEvents(TimeMS _delta);
    void HandleEvent(EngineEvent _event);

private:
    CommandLineArgs m_CommandLineArgs;
    EngineConfig m_EngineConfig;

    std::shared_ptr<sf::RenderWindow> m_Window = nullptr;

    std::shared_ptr<World> m_World = nullptr;
    std::shared_ptr<World> m_RequestedWorld = nullptr;

    std::shared_ptr<RenderHandler> m_RenderHandler = nullptr;
    std::shared_ptr<UIDisplay> m_UIDisplay = nullptr;
    std::shared_ptr<Input> m_Input = nullptr;
    
    EventHandler<EngineEvent> m_EventHandler;

    u64 m_FrameCount = 0;

    bool m_IsGameplayRunning = false;

    StandardInputTask m_StandardInputTask;
};
