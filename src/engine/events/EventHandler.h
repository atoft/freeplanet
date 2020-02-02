//
// Created by alastair on 02/10/18.
//

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
