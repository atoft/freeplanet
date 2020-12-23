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

#include <vector>
#include <mutex>

template <typename T>
class EventHandler
{
private:
    std::vector<T> m_PendingEvents;
    std::mutex m_EventMutex;

public:
    std::vector<T> PopEvents();
    void PushEvent(T _event);

};

template <typename T>
std::vector<T> EventHandler<T>::PopEvents()
{
    std::lock_guard<std::mutex> lock(m_EventMutex);

    std::vector<T> result = m_PendingEvents;
    m_PendingEvents.clear();

    return result;
}

template <typename T>
void EventHandler<T>::PushEvent(T _event)
{
    std::lock_guard<std::mutex> lock(m_EventMutex);

    m_PendingEvents.push_back(_event);
}
