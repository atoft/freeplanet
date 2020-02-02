//
// Created by alastair on 03/10/18.
//

#include "StandardInputTask.h"

#include <iostream>

#include <src/engine/Engine.h>

void StandardInputTask::Execute()
{
    bool quitPending = false;

    while(!quitPending)
    {
        std::vector<EngineEvent> events = m_EventHandler.PopEvents();
        for(const EngineEvent& event : events)
        {
            switch(event.GetType())
            {
                case EngineEvent::Type::OnQuit:
                {
                    quitPending = true;
                    break;
                }
                default:
                    break;
            }
        }

        std::string input;

        std::cin >> input;

        auto parsedEvent = EngineEvent::ParseEvent(input);
        if(parsedEvent)
        {
            Engine::GetInstance().AddEvent(*parsedEvent);
        }
    }
}


