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
