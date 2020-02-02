//
// Created by alastair on 03/10/18.
//


#pragma once

#include <thread>

#include <src/engine/events/EngineEvent.h>
#include <src/engine/events/EventHandler.h>

class Task
{
private:
    std::thread m_Thread;
    bool m_IsActive;
protected:
    EventHandler<EngineEvent> m_EventHandler;
    virtual void Execute() = 0;
public:
    Task();
    Task(Task& _other) = delete;
    ~Task();

    void Start();
    void HandleEvent(EngineEvent _event);
};



