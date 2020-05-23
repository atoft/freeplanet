//
// Created by alastair on 17/01/2020.
//

#pragma once

#include <src/tools/globals.h>
#include <src/engine/inspection/InspectionContext.h>

class InspectionContext;

void Inspect(std::string _name, u32& _target, InspectionContext& context);
void Inspect(std::string _name, s32& _target, InspectionContext& context);
void Inspect(std::string _name, f32& _target, InspectionContext& context);

void Inspect(std::string _name, bool& _target, InspectionContext& context);

void Inspect(std::string _name, glm::vec2& _target, InspectionContext& context);
void Inspect(std::string _name, glm::vec3& _target, InspectionContext& context);
void Inspect(std::string _name, glm::ivec2& _target, InspectionContext& context);
void Inspect(std::string _name, glm::ivec3& _target, InspectionContext& context);

template <typename EnumType, typename = std::enable_if_t<std::is_enum<EnumType>::value>>
void Inspect(std::string _name, EnumType& _enum, InspectionContext& _context)
{
    _context.Enum(_name, _enum);
}
