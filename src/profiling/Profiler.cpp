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

#include "Profiler.h"

#include <algorithm>
#include <vector>

#ifdef ENABLE_FUNCTION_PROFILING

ScopedProfilerTimer::ScopedProfilerTimer(std::string _name)
{
    m_Timer.Start();
    Profiler::GetInstance().RegisterScope(std::move(_name));
}

ScopedProfilerTimer::~ScopedProfilerTimer()
{
    Profiler::GetInstance().RecordScopeTime(m_Timer.GetElapsedTime());
}

void Profiler::FunctionProfileData::GetChildrenInfo(u32 _depth, std::string& _outText, std::vector<TimeMS>& _outTimes) const
{
    std::string indentation;
    for (u32 i = 0; i < _depth; ++i)
    {
        indentation += " > ";
    }

    for (const auto& data : m_ChildDatas)
    {
        _outText += indentation + data.first
                + (data.second.m_CallsLastFrame > 1 ? " (" + std::to_string(data.second.m_CallsLastFrame) + " calls)" : "")
                + "\n";

        _outTimes.push_back(data.second.m_DurationLastFrame * 1000.f);

        data.second.GetChildrenInfo(_depth + 1, _outText, _outTimes);
    }
}

void Profiler::FunctionProfileData::Cleanup()
{
    m_CallsLastFrame = 0;
    m_DurationLastFrame = 0.f;

    for (auto& data : m_ChildDatas)
    {
        data.second.Cleanup();
    }
}

void Profiler::RegisterScope(std::string _name)
{
    // We never delete entries from the map - we assume that every function will be called every frame for now.

    FunctionProfileData& data = m_CurrentScope->m_ChildDatas[std::move(_name)];
    data.m_ParentScope = m_CurrentScope;
    ++data.m_CallsLastFrame;

    m_CurrentScope = &data;
}

void Profiler::RecordScopeTime(TimeSeconds _time)
{
    m_CurrentScope->m_DurationLastFrame += _time;
    m_CurrentScope = m_CurrentScope->m_ParentScope;
}

#endif // ENABLE_FUNCTION_PROFILING

Profiler& Profiler::GetInstance()
{
    static Profiler instance;
    return instance;
}

void Profiler::Update(TimeMS _delta)
{
    m_CurrentFPS = 1000.f / _delta;

#ifdef ENABLE_FUNCTION_PROFILING
    m_LastFrameDebugText = "";
    m_LastFrameDebugTimes.clear();

    m_ProfileData.GetChildrenInfo(0, m_LastFrameDebugText, m_LastFrameDebugTimes);

    m_ProfileData.Cleanup();
#endif // ENABLE_FUNCTION_PROFILING
}

f32 Profiler::GetCurrentFPS() const
{
    return m_CurrentFPS;
}

void Profiler::GetDebugInfo(std::string& _outText, std::vector<TimeMS>& _outTimes) const
{
#ifdef ENABLE_FUNCTION_PROFILING
    _outText = m_LastFrameDebugText;
    _outTimes = m_LastFrameDebugTimes;
#else
    _outText = "Function profiling is not available in this build.";
#endif // ENABLE_FUNCTION_PROFILING
}


