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

#include <glm/glm.hpp>
#include <vector>

#include <src/tools/globals.h>
#include <src/engine/InputTypes.h>

class InspectionContext;

struct GraphicsConfig
{
    glm::ivec2 m_Resolution = glm::ivec2(1280, 720);
    bool m_IsFullscreen = true;

    u32 m_AntialiasingLevel = 4;
    bool m_IsVsyncEnabled = true;

    f32 m_FieldOfViewDegrees = 90.f;
};

struct InputConfig
{
    std::vector<InputKeyMapping> m_KeyMappings;
};

struct EngineConfig
{
    GraphicsConfig m_GraphicsConfig;
    InputConfig m_InputConfig;
};

void Inspect(std::string _name, GraphicsConfig& _target, InspectionContext& _context);
void Inspect(std::string _name, InputConfig& _target, InspectionContext& _context);
void Inspect(std::string _name, EngineConfig& _target, InspectionContext& _context);
