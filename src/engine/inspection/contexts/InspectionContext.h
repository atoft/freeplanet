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

#include <optional>
#include <string>
#include <vector>
#include <variant>
#include <type_traits>

#include <src/tools/globals.h>
#include <src/engine/inspection/contexts/FromBinaryInspectionContext.h>
#include <src/engine/inspection/contexts/ToBinaryInspectionContext.h>
#include <src/engine/inspection/contexts/TextInspectionContext.h>
#include <src/engine/inspection/InspectionTypes.h>
#include <src/engine/inspection/TypeInfo.h>
#include <src/tools/STL.h>
#include <src/tools/StringHelpers.h>

using InspectionVariant = std::variant<TextInspectionContext, FromBinaryInspectionContext, ToBinaryInspectionContext>;

class InspectionContext
{
    friend class InspectionHelpers;
    
public:
    void Struct(std::string _name, InspectionType _type, u32 _version, InspectionStructRequirements _requirements = InspectionStructRequirements::RequireExactMatch);
    void EndStruct();

    void U32(std::string _name, u32& _value);
    void S32(std::string _name, s32& _value);
    void F32(std::string _name, f32& _value);

    void Bool(std::string _name, bool& _value);

    template <typename EnumType, typename = std::enable_if_t<std::is_enum<EnumType>::value>>
    void Enum(std::string _name, EnumType& _value);

    template <typename ElementType>
    void Vector(std::string _name, std::vector<ElementType>& _value);

private:
    InspectionVariant m_Variant;
};

template<typename EnumType, typename>
void InspectionContext::Enum(std::string _name, EnumType& _value)
{
    std::visit([&](auto&& _var){ _var.Enum(_name, _value); }, m_Variant);
}

template<typename ElementType>
void InspectionContext::Vector(std::string _name, std::vector<ElementType>& _value)
{
    std::visit([&](auto&& _var){ _var.Vector(_name, _value); }, m_Variant);
}

