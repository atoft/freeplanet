//
// Created by alastair on 03/10/18.
//

#include "Task.h"

Task::Task()
    : m_IsActive(false)
{
}

Task::~Task()
{
    if(m_Thread.joinable())
    {
        m_Thread.join();
    }
}

void Task::Start()
{
    m_Thread = std::thread([this]{this->Execute();});
    m_IsActive = true;
}

void Task::HandleEvent(EngineEvent _event)
{
    if(!m_IsActive)
    {
        return;
    }

    m_EventHandler.PushEvent(_event);
}
