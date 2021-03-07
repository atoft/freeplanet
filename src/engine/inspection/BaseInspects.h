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

#include <src/tools/globals.h>
#include <src/engine/inspection/contexts/InspectionContext.h>

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

template <typename... VariantTypes>
void Inspect(std::string _name, std::variant<VariantTypes...>& _target, InspectionContext& _context)
{
    _context.Variant(_name, _target);
}
