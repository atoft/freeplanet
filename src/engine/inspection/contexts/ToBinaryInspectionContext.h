/*
 * Copyright 2021 Alastair Toft
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
#include <type_traits>

#include <src/tools/globals.h>
#include <src/engine/inspection/InspectionTypes.h>
#include <src/engine/inspection/TypeInfo.h>
#include <src/tools/STL.h>
#include <src/tools/StringHelpers.h>

class ToBinaryInspectionContext
{
    friend class InspectionHelpers;

public:
    void Struct(std::string _name, InspectionType _type, u32 _version, InspectionStructRequirements _requirements = InspectionStructRequirements::RequireExactMatch);
    void EndStruct();

    void U32(std::string _name, const u32& _value);
    void S32(std::string _name, const s32& _value);
    void F32(std::string _name, const f32& _value);

    void Bool(std::string _name, const bool& _value);

    template <typename EnumType, typename = std::enable_if_t<std::is_enum<EnumType>::value>>
    void Enum(std::string _name, const EnumType& _value);

    template <typename ElementType>
    void Vector(std::string _name, const std::vector<ElementType>& _value);

private:
    void WriteU32(u32 _value);
    
    std::vector<u8>* m_Buffer = nullptr;
    class InspectionContext* m_Outer = nullptr;
};

template <typename EnumType, typename>
void ToBinaryInspectionContext::Enum(std::string _name, const EnumType& _value)
{

}

template <typename ElementType>
void ToBinaryInspectionContext::Vector(std::string _name, const std::vector<ElementType>& _value)
{
}


