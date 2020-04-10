//
// Created by alastair on 16/10/19.
//

#include "UIProfilingDebug.h"

#include <src/profiling/Profiler.h>

void UIProfilingDebug::Init(const Profiler* _profiler)
{
    m_Profiler = _profiler;
}

void UIProfilingDebug::Draw(TimeMS, UIDrawInterface& _display, const World*)
{
    if (m_Profiler != nullptr)
    {
        _display.DrawString(glm::vec2(1500,20), std::to_string(m_Profiler->GetCurrentFPS()) + " fps", 24.f);

        std::string debugText;
        std::vector<TimeMS> debugTimes;

        m_Profiler->GetDebugInfo(debugText, debugTimes);

        _display.DrawString(glm::vec2(80,80), debugText, 16.f);

        std::string debugTimesString;

        glm::vec2 barCurrentPosition = glm::vec2(500,80);

        constexpr f32 BAR_MAX_WIDTH = 400.f / 16.67;
        constexpr f32 BAR_HEIGHT = 16.f;
        constexpr f32 BAR_SPACING = 3.f;

        for (const TimeMS time : debugTimes)
        {
            debugTimesString += std::to_string(time) + " ms\n";

            _display.DrawRectangle(barCurrentPosition, glm::vec2(time * BAR_MAX_WIDTH, BAR_HEIGHT), Color(0,1,1,1));
            barCurrentPosition.y += BAR_HEIGHT + BAR_SPACING;
        }

        _display.DrawString(glm::vec2(400,80), debugTimesString, 16.f);
    }
}
