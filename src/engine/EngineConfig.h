//
// Created by alastair on 20/01/2020.
//

#pragma once

#include <glm/glm.hpp>

#include <src/tools/globals.h>

class InspectionContext;

struct EngineConfig
{
    glm::ivec2 m_Resolution;
    bool m_IsFullscreen = true;

    u32 m_AntialiasingLevel = 4;
    bool m_IsVsyncEnabled = true;
};

void Inspect(std::string _name, EngineConfig& _target, InspectionContext& _context);
