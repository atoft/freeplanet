//
// Created by alastair on 06/05/19.
//

#include "Stopwatch.h"

#include <src/world/World.h>

Stopwatch::Stopwatch(const World* _world)
    : m_World(_world) {}

void Stopwatch::Start()
{
    m_Clock.restart();
    m_IsStarted = true;
    m_DurationSeconds = 0.f;
}

void Stopwatch::Start(f32 duration)
{
    m_Clock.restart();
    m_IsStarted = true;
    m_DurationSeconds = duration;
}

bool Stopwatch::IsEndless() const
{
    return m_DurationSeconds <= 0.f;
}

bool Stopwatch::IsRunning() const
{
    return m_IsStarted && !IsFinished();
}

bool Stopwatch::IsFinished() const
{
    return m_IsStarted
        && !IsEndless()
        && GetElapsedTime()>= m_DurationSeconds;
}

void Stopwatch::Stop()
{
    m_IsStarted = false;
}

f32 Stopwatch::GetElapsedTime() const
{
    const f32 timeScale = m_World != nullptr ? m_World->GetTimeScale() : 1.f;

    return m_Clock.getElapsedTime().asSeconds() * timeScale;
}
