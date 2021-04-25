/*
 * Copyright 2017-2021 Alastair Toft
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
#include <src/engine/inspection/contexts/GetTypeNameInspectionContext.h>
#include <src/engine/inspection/InspectionTypes.h>
#include <src/engine/inspection/VariantFromName.h>
#include <src/engine/inspection/TypeInfo.h>
#include <src/tools/STL.h>
#include <src/tools/StringHelpers.h>

class FromTextInspectionContext
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
    std::optional<std::string> ParseValueAndSkip(std::string _name, std::string::const_iterator& _it, std::string::const_iterator _end);
    void SkipWhitespace(std::string::const_iterator& _it, std::string::const_iterator _end);
    bool SkipSingleToken(std::string::const_iterator& _it, std::string::const_iterator _end, const std::string& _token, bool _allowFailure = false);
    void AddError(std::string _error);
    void AddWarning(std::string _warning);
    u32 GetLineNumber() const;

    struct InspectionStackEntry
    {
        // Container elements are different in text as they don't have associated names.
        bool m_InsideContainer = false;

        InspectionStructRequirements m_StructRequirements = InspectionStructRequirements::RequireExactMatch;
        bool m_SkipThisLevel = false;
    };

    // Maintain information about where in the struct we currently are.
    std::vector<InspectionStackEntry> m_Stack;

    bool m_Finished = false;
    InspectionResult m_Result = InspectionResult::Success;
    std::string m_ErrorMessage = "";
    std::string m_WarningMessage = "";

    std::string::const_iterator m_TextBegin;
    std::string::const_iterator m_TextIt;
    std::string::const_iterator m_TextEnd;
};

template<typename EnumType, typename> void FromTextInspectionContext::Enum(std::string _name, EnumType& _value)
{
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    const std::optional<std::string> valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

    if (!valueString)
    {
        return;
    }

    _value = STL::FromString<EnumType>(*valueString);
}

template<typename ElementType> void FromTextInspectionContext::Vector(std::string _name, std::vector<ElementType>& _value)
{
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    if (!m_Stack.back().m_InsideContainer)
    {
        const std::string expectedPropertyIdentifier = _name + ":";
        assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, expectedPropertyIdentifier));
        m_TextIt += expectedPropertyIdentifier.size();
    }
    SkipWhitespace(m_TextIt, m_TextEnd);
    assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, "["));
    ++m_TextIt;
    SkipWhitespace(m_TextIt, m_TextEnd);

    m_Stack.push_back({true, InspectionStructRequirements::RequireExactMatch});

    if (!StringHelpers::StartsWith(m_TextIt, m_TextEnd, "]"))
    {
        while (!StringHelpers::StartsWith(m_TextIt, m_TextEnd, ";"))
        {
            ElementType element;
            Inspect(_name, element, *this);

            // If there was something wrong with the inspection of the element, give up
            // to make sure we aren't stuck in place with our iteration.
            if (m_Finished)
            {
                return AddError("Failed to read vector");
            }

            _value.push_back(element);

            SkipWhitespace(m_TextIt, m_TextEnd);
            if (m_TextIt == m_TextEnd) return AddError("Unexpected end of file");
        }
    }
    else
    {
        // Skip the empty list.
        SkipSingleToken(m_TextIt, m_TextEnd, "]");
    }

    assert(m_Stack.back().m_InsideContainer);
    m_Stack.pop_back();

    SkipSingleToken(m_TextIt, m_TextEnd, ";");
    SkipWhitespace(m_TextIt, m_TextEnd);
}

template <typename... VariantTypes>
void FromTextInspectionContext::Variant(std::string _name, std::variant<VariantTypes...>& _value)
{
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    if (!m_Stack.back().m_InsideContainer)
    {
        const std::string expectedPropertyIdentifier = _name + ":";
        assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, expectedPropertyIdentifier));
        m_TextIt += expectedPropertyIdentifier.size();
    }

    SkipWhitespace(m_TextIt, m_TextEnd);
    if (!SkipSingleToken(m_TextIt, m_TextEnd, "<")) return;

    auto endOfTypeNameIt = StringHelpers::Find(m_TextIt, m_TextEnd, ">");
    const std::string typeName = std::string(m_TextIt, endOfTypeNameIt);
    bool didNameMatch = false;
    _value = InspectionVariants::VariantFromName<std::variant<VariantTypes...>>(typeName, didNameMatch);

    if (!didNameMatch)
    {
        AddError("Variant name did not match any alternative for " + _name);
        return;
    }

    m_TextIt = endOfTypeNameIt;
    if (!SkipSingleToken(m_TextIt, m_TextEnd, ">")) return;
    SkipWhitespace(m_TextIt, m_TextEnd);
    if (!SkipSingleToken(m_TextIt, m_TextEnd, "[")) return;

    m_Stack.push_back({true, InspectionStructRequirements::RequireExactMatch});

    std::visit([&](auto&& val) { Inspect("", val, *this);}, _value);

    assert(m_Stack.back().m_InsideContainer);
    m_Stack.pop_back();

    // TODO Add a unit test with a variant inside a vector.

    // TODO Copy-paste: This part of the parsing should be shared with other methods.
    std::string endOfValueString = ";";

    if (m_Stack.back().m_InsideContainer)
    {
        // Find the closest token of , or ].
        const std::string::const_iterator nextComma = StringHelpers::Find(m_TextIt, m_TextEnd, ",");
        const std::string::const_iterator nextClosingSquareBrace = StringHelpers::Find(m_TextIt, m_TextEnd, "]");

        if (nextComma < nextClosingSquareBrace)
        {
            endOfValueString = ",";
        }
        else
        {
            endOfValueString = "]";
        }
    }

    if (!SkipSingleToken(m_TextIt, m_TextEnd, endOfValueString)) return;

    SkipWhitespace(m_TextIt, m_TextEnd);
}


