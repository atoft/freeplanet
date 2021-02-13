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

#include "InspectionContext.h"

void InspectionContext::Struct(std::string _name, InspectionType _type, u32 _version, InspectionStructRequirements _requirements)
{
    std::visit([&](auto&& _var){ _var.Struct(_name, _type, _version, _requirements); }, m_Variant);
}

void InspectionContext::EndStruct()
{
    std::visit([&](auto&& _var){ _var.EndStruct(); }, m_Variant);
}

void InspectionContext::U32(std::string _name, u32& _value)
{
    std::visit([&](auto&& _var){ _var.U32(_name, _value); }, m_Variant);
}

void InspectionContext::S32(std::string _name, s32& _value)
{
    std::visit([&](auto&& _var){ _var.S32(_name, _value); }, m_Variant);
}

void InspectionContext::F32(std::string _name, f32& _value)
{
    std::visit([&](auto&& _var){ _var.F32(_name, _value); }, m_Variant);
}

void InspectionContext::Bool(std::string _name, bool& _value)
{
    std::visit([&](auto&& _var){ _var.Bool(_name, _value); }, m_Variant);
}

