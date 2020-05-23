//
// Created by alastair on 20/01/2020.
//

#include "EngineConfig.h"

#include <src/engine/inspection/BaseInspects.h>
#include <src/engine/inspection/TypeInfo.h>
#include <src/engine/inspection/InspectionContext.h>

void Inspect(std::string _name, GraphicsConfig& _target, InspectionContext& _context)
{
    _context.Struct("GraphicsConfig", InspectionType::GraphicsConfig, 0);

    Inspect("Resolution", _target.m_Resolution, _context);
    Inspect("IsFullscreen", _target.m_IsFullscreen, _context);
    Inspect("AntialiasingLevel", _target.m_AntialiasingLevel, _context);
    Inspect("IsVsyncEnabled", _target.m_IsVsyncEnabled, _context);

    Inspect("FieldOfViewDegrees", _target.m_FieldOfViewDegrees, _context);

    _context.EndStruct();
}

void Inspect(std::string _name, EngineConfig& _target, InspectionContext& _context)
{
    _context.Struct("EngineConfig", InspectionType::EngineConfig, 0);

    Inspect("GraphicsConfig", _target.m_GraphicsConfig, _context);

    _context.EndStruct();
}