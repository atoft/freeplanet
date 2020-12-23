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

#include <map>

#include <src/tools/globals.h>
#include <src/engine/Stopwatch.h>
#include <vector>

#ifndef NDEBUG
#define ENABLE_FUNCTION_PROFILING
#endif

#ifdef ENABLE_FUNCTION_PROFILING
struct ScopedProfilerTimer
{
    explicit ScopedProfilerTimer(std::string _name);
    ~ScopedProfilerTimer();

    Stopwatch m_Timer;
};
#endif // ENABLE_FUNCTION_PROFILING


#ifdef ENABLE_FUNCTION_PROFILING
// @CrossPlatform __PRETTY_FUNCTION__ is non-standard C++ but Clang supports it.
#define ProfileCurrentFunction() ScopedProfilerTimer scopedProfilerTimer = ScopedProfilerTimer(StripFunctionString(__PRETTY_FUNCTION__))
#define ProfileCurrentScope(_name) ScopedProfilerTimer scopedProfilerTimer##__name = ScopedProfilerTimer(StripFunctionString(__PRETTY_FUNCTION__) + " - " +_name)
#else
#define ProfileCurrentFunction()
#define ProfileCurrentScope(_name)
#endif // ENABLE_FUNCTION_PROFILING

class Profiler
{
public:
    Profiler(Profiler const&) = delete;
    void operator=(Profiler const&) = delete;

    static Profiler& GetInstance();

    void Update(TimeMS _delta);

    f32 GetCurrentFPS() const;
    void GetDebugInfo(std::string& _outText, std::vector<TimeMS>& _outTimes) const;

#ifdef ENABLE_FUNCTION_PROFILING
    void RegisterScope(std::string _name);
    void RecordScopeTime(TimeSeconds _time);

private:
    // We have a tree made of maps (!) to support profiling nested functions.
    // Probably cleanup to use some array-based data structure.
    struct FunctionProfileData
    {
        TimeSeconds m_DurationLastFrame = 0.f;
        u32 m_CallsLastFrame = 0;
        std::map<std::string, FunctionProfileData> m_ChildDatas;
        FunctionProfileData* m_ParentScope = nullptr;

        void GetChildrenInfo(u32 _depth, std::string& _outText, std::vector<TimeMS>& _outTimes) const;
        void Cleanup();
    };

private:
    FunctionProfileData m_ProfileData;
    FunctionProfileData* m_CurrentScope = &m_ProfileData;

    std::string m_LastFrameDebugText;
    std::vector<TimeMS> m_LastFrameDebugTimes;

#endif // ENABLE_FUNCTION_PROFILING

private:
    Profiler() = default;

private:
    f32 m_CurrentFPS = 0.f;
};
