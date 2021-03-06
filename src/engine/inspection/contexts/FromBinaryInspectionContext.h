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
#include <variant>
#include <vector>
#include <type_traits>

#include <src/tools/globals.h>
#include <src/engine/inspection/InspectionTypes.h>
#include <src/engine/inspection/InspectionUtils.h>
#include <src/engine/inspection/TypeInfo.h>
#include <src/tools/STL.h>
#include <src/tools/StringHelpers.h>

class FromBinaryInspectionContext
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

    template <typename... VariantTypes>
    void Variant(std::string _name, std::variant<VariantTypes...>& _value);

private:
    std::optional<u32> ReadU32();

    void AddError(const std::string& _error);
    void AddWarning(const std::string& _warning);
    
    std::vector<u8>::const_iterator m_It;

    std::vector<u8>::const_iterator m_Begin;
    std::vector<u8>::const_iterator m_End;

    bool m_Finished = false;
    u32 m_Depth = 0;
    
    FromBinaryInspectionResult m_Result = FromBinaryInspectionResult::Success;
    std::string m_ErrorMessage = "";
    std::string m_WarningMessage = "";
};

template <typename EnumType, typename>
void FromBinaryInspectionContext::Enum(std::string _name, EnumType& _value)
{
    if (m_Finished)
    {
        return;
    }

    // TODO Could support different sizes for enums.
    const std::optional<u32> readValue = ReadU32();

    if (!readValue)
    {
        AddError("Read failed at " + _name);
        return;
    }

    _value = static_cast<EnumType>(*readValue);
}

template <typename ElementType>
void FromBinaryInspectionContext::Vector(std::string _name, std::vector<ElementType>& _value)
{
    if (m_Finished)
    {
        return;
    }

    // TODO Could support different sizes for enums.
    const std::optional<u32> readValue = ReadU32();

    if (!readValue)
    {
        AddError("Read failed at " + _name);
        return;
    }

    const u32 vectorSize = *readValue;

    _value.reserve(vectorSize);
    
    for (u32 idx = 0; idx < vectorSize; ++idx)
    {
        ElementType& element =_value.emplace_back();
        Inspect(_name + "[" + std::to_string(idx) + "]", element, *this);

        if (m_Finished)
        {
            return;
        }
    }

    assert(_value.size() == vectorSize);
}

template <typename... VariantTypes>
void FromBinaryInspectionContext::Variant(std::string _name, std::variant<VariantTypes...>& _value)
{
    if (m_Finished)
    {
        return;
    }
    
    const std::optional<u32> readValue = ReadU32();

    if (!readValue)
    {
        AddError("Read failed at " + _name);
        return;
    }

    const u32 variantIdx = *readValue;

    _value = InspectionUtils::VariantFromIndex<std::variant<VariantTypes...>>(variantIdx);

    std::visit([&](auto&& _var){ Inspect(_name + ".[VariantContents]", _var, *this); }, _value);
}
