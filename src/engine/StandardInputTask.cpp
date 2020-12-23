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


