//
// Created by alastair on 06/05/19.
//

#pragma once

#include <SFML/System/Clock.hpp>

#include <src/tools/globals.h>

class World;

class Stopwatch
{
public:
    Stopwatch() = default;
    explicit Stopwatch(const World* _world);
    void Start();
    void Start(f32 duration);

    bool IsEndless() const;
    bool IsRunning() const;
    bool IsFinished() const;
    void Stop();

    f32 GetElapsedTime() const;

private:
    const World* m_World = nullptr;
    bool m_IsStarted = false;
    sf::Clock m_Clock;
    f32 m_DurationSeconds = 0.f;

};
