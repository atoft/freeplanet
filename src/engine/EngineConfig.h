//
// Created by alastair on 20/01/2020.
//

#pragma once

#include <glm/glm.hpp>

#include <src/tools/globals.h>
#include "InputTypes.h"

class InspectionContext;

struct GraphicsConfig
{
    glm::ivec2 m_Resolution;
    bool m_IsFullscreen = true;

    u32 m_AntialiasingLevel = 4;
    bool m_IsVsyncEnabled = true;

    f32 m_FieldOfViewDegrees = 90.f;
};

struct EngineConfig
{
    GraphicsConfig m_GraphicsConfig;
};

void Inspect(std::string _name, GraphicsConfig& _target, InspectionContext& _context);
void Inspect(std::string _name, EngineConfig& _target, InspectionContext& _context);
