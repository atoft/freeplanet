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

#include "EngineConfig.h"

#include <src/engine/inspection/BaseInspects.h>
#include <src/engine/inspection/TypeInfo.h>
#include <src/engine/inspection/InspectionContext.h>
#include <src/engine/inspection/VectorInspect.h>

void Inspect(std::string _name, GraphicsConfig& _target, InspectionContext& _context)
{
    _context.Struct("GraphicsConfig", InspectionType::GraphicsConfig, 0, InspectionStructRequirements::AllowMissingValues);

    Inspect("Resolution", _target.m_Resolution, _context);
    Inspect("IsFullscreen", _target.m_IsFullscreen, _context);
    Inspect("AntialiasingLevel", _target.m_AntialiasingLevel, _context);
    Inspect("IsVsyncEnabled", _target.m_IsVsyncEnabled, _context);

    Inspect("FieldOfViewDegrees", _target.m_FieldOfViewDegrees, _context);

    _context.EndStruct();
}

void Inspect(std::string _name, InputConfig& _target, InspectionContext& _context)
{
    _context.Struct("InputConfig", InspectionType::InputConfig, 0, InspectionStructRequirements::AllowMissingValues);

    Inspect("KeyMappings", _target.m_KeyMappings, _context);

    _context.EndStruct();
}

void Inspect(std::string _name, EngineConfig& _target, InspectionContext& _context)
{
    _context.Struct("EngineConfig", InspectionType::EngineConfig, 0, InspectionStructRequirements::AllowMissingValues);

    Inspect("GraphicsConfig", _target.m_GraphicsConfig, _context);
    Inspect("InputConfig", _target.m_InputConfig, _context);

    _context.EndStruct();
}