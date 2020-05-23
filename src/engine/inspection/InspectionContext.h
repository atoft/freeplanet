//
// Created by alastair on 17/01/2020.
//

#pragma once

#include <string>
#include <vector>
#include <type_traits>

#include <src/tools/globals.h>
#include <src/engine/inspection/TypeInfo.h>
#include <src/tools/STL.h>
#include <src/tools/StringHelpers.h>

class InspectionContext
{
    friend class InspectionHelpers;

public:
    void Struct(std::string _name, InspectionType _type, u32 _version);
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
    static InspectionType ToInspectionType(std::string _typeName);
    static std::string InspectionTypeToString(InspectionType _type);

    std::string ParseValueAndSkip(std::string _name, std::string::const_iterator& _it, std::string::const_iterator _end);
    void AppendNameAndValue(std::string _name, std::string _value);
    void AppendNewlineWithIndent();
    void SkipWhitespace(std::string::const_iterator& _it, std::string::const_iterator _end);
    void SkipSingleToken(std::string::const_iterator& _it, std::string::const_iterator _end, const std::string& _token);

    enum class Operation
    {
        FromText,
        ToText,
        FromBinary,
        ToBinary
    };

    static std::vector<TypeInfo> ms_TypeInfos;

    Operation m_Operation = Operation::ToText;
    
    // How many nested structs are we in.
    u32 m_Depth = 0;

    // At each level of nesting, are the values elements in a container.
    // Container elements are different in text as the don't have associated names.
    std::vector<bool> m_InsideContainer;

    // TODO placeholder.

    // ToText ///////////////////////////////
    std::string* m_TextBuffer;

    // FromText /////////////////////////////
    std::string::const_iterator m_TextIt;
    std::string::const_iterator m_TextEnd;
};

template <typename EnumType, typename>
void InspectionContext::Enum(std::string _name, EnumType& _value)
{
    switch (m_Operation)
    {
    case Operation::ToText:
    {
        AppendNameAndValue(_name, ToString(_value));

        break;
    }
    case Operation::FromText:
    {
        std::string valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

        _value = STL::FromString<EnumType>(valueString);

        break;
    }
    default:
        break;
    }
}

template <typename ElementType>
void InspectionContext::Vector(std::string _name, std::vector<ElementType>& _value)
{
    switch (m_Operation)
    {
        case Operation::ToText:
        {
            AppendNewlineWithIndent();
            if (!m_InsideContainer.back())
            {
                *m_TextBuffer += _name + ": ";
            }

            *m_TextBuffer += "[";

            m_InsideContainer.push_back(true);
            for (u32 elementIdx = 0; elementIdx < _value.size(); ++elementIdx)
            {
                Inspect(_name, _value[elementIdx], *this);

                if (elementIdx != _value.size() - 1)
                {
                    *m_TextBuffer += ", ";
                }
            }
            assert(m_InsideContainer.back());
            m_InsideContainer.pop_back();

            *m_TextBuffer += "];";

            break;
        }
        case Operation::FromText:
        {
            if (!m_InsideContainer.back())
            {
                const std::string expectedPropertyIdentifier = _name + ":";
                assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, expectedPropertyIdentifier));
                m_TextIt += expectedPropertyIdentifier.size();
            }
            SkipWhitespace(m_TextIt, m_TextEnd);
            assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, "["));
            ++m_TextIt;
            SkipWhitespace(m_TextIt, m_TextEnd);

            m_InsideContainer.push_back(true);

            if (!StringHelpers::StartsWith(m_TextIt, m_TextEnd, "]"))
            {
                while (!StringHelpers::StartsWith(m_TextIt, m_TextEnd, ";"))
                {
                    ElementType element;
                    Inspect(_name, element, *this);
                    _value.push_back(element);

                    SkipWhitespace(m_TextIt, m_TextEnd);
                }
            }
            else
            {
                // Skip the empty list.
                SkipSingleToken(m_TextIt, m_TextEnd, "]");
            }

            assert(m_InsideContainer.back());
            m_InsideContainer.pop_back();

            SkipSingleToken(m_TextIt, m_TextEnd, ";");
            SkipWhitespace(m_TextIt, m_TextEnd);

            break;
        }
        default:
            break;
    }
}
