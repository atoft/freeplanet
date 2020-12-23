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

#include "UIProfilingDebug.h"

#include <src/profiling/Profiler.h>

void UIProfilingDebug::InitProfiler(const Profiler* _profiler)
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
