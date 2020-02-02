//
// Created by alastair on 16/10/19.
//

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
