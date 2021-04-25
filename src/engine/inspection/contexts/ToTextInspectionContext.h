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

class ToTextInspectionContext
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
    void AppendNameAndValue(std::string _name, std::string _value);
    void AppendNewlineWithIndent();

    struct InspectionStackEntry
    {
        // Container elements are different in text as they don't have associated names.
        bool m_InsideContainer = false;
    };

    // Maintain information about where in the struct we currently are.
    std::vector<InspectionStackEntry> m_Stack;

    bool m_Finished = false;
    InspectionResult m_Result = InspectionResult::Success;

    std::string* m_TextBuffer;
};

template<typename EnumType, typename> void ToTextInspectionContext::Enum(std::string _name, EnumType& _value)
{
    AppendNameAndValue(_name, ToString(_value));
}

template<typename ElementType> void ToTextInspectionContext::Vector(std::string _name, std::vector<ElementType>& _value)
{
    AppendNewlineWithIndent();
    if (!m_Stack.back().m_InsideContainer)
    {
        *m_TextBuffer += _name + ": ";
    }

    *m_TextBuffer += "[";

    m_Stack.push_back({true});
    for (u32 elementIdx = 0; elementIdx < _value.size(); ++elementIdx)
    {
        Inspect(_name, _value[elementIdx], *this);

        if (elementIdx != _value.size() - 1)
        {
            *m_TextBuffer += ", ";
        }
    }
    assert(m_Stack.back().m_InsideContainer);
    m_Stack.pop_back();

    *m_TextBuffer += "];";
}

template <typename... VariantTypes>
void ToTextInspectionContext::Variant(std::string _name, std::variant<VariantTypes...>& _value)
{
    AppendNewlineWithIndent();
    if (!m_Stack.back().m_InsideContainer)
    {
        *m_TextBuffer += _name + ": ";
    }

    GetTypeNameInspectionContext typeNameContext;
    std::visit([&typeNameContext](auto&& val) { Inspect("", val, typeNameContext);}, _value);
    *m_TextBuffer += "<" + typeNameContext.m_Result + ">";

    *m_TextBuffer += "[";
    m_Stack.push_back({true});

    std::visit([this](auto&& val) { Inspect("", val, *this);}, _value);

    assert(m_Stack.back().m_InsideContainer);
    m_Stack.pop_back();

    *m_TextBuffer += "]";

    if (!m_Stack.back().m_InsideContainer)
    {
        *m_TextBuffer += ";";
    }
}
