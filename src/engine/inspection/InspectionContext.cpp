//
// Created by alastair on 17/01/2020.
//

#include "InspectionContext.h"

#include <src/tools/StringHelpers.h>

std::vector<TypeInfo> InspectionContext::ms_TypeInfos =
        {
            {"vec2", InspectionType::vec2},
            {"vec3", InspectionType::vec3},

            {"ivec2", InspectionType::ivec2},
            {"ivec3", InspectionType::ivec3},

            {"TestStruct", InspectionType::TestStruct},
            {"TestSubStruct", InspectionType::TestSubStruct},

            {"EngineConfig", InspectionType::EngineConfig},
        };

InspectionType InspectionContext::ToInspectionType(std::string _typeName)
{
    for (const TypeInfo& info : ms_TypeInfos)
    {
        if (info.m_Name == _typeName)
        {
            return info.m_Type;
        }
    }

    return InspectionType::Invalid;
}

std::string InspectionContext::ToString(InspectionType _type)
{
    for (const TypeInfo& info : ms_TypeInfos)
    {
        if (info.m_Type == _type)
        {
            return info.m_Name;
        }
    }

    return "INVALID";
}

void SkipWhitespace(std::string::const_iterator& _it, std::string::const_iterator _end)
{
    for (; _it != _end; ++_it)
    {
        if (*_it != ' ' && *_it != '\n' && *_it != '\t')
        {
            break;
        }
    }
}

std::string ParseValueAndSkip(std::string _name, std::string::const_iterator& _it, std::string::const_iterator _end)
{
    const std::string expectedPropertyIdentifier = _name + ":";
    assert(StringHelpers::StartsWith(_it, _end, expectedPropertyIdentifier));
    _it += expectedPropertyIdentifier.size();

    SkipWhitespace(_it, _end);
    assert(_it != _end);

    const std::string::const_iterator endOfLine = StringHelpers::Find(_it, _end, "\n");
    assert(endOfLine != _end);

    std::string valueString = std::string(_it, endOfLine);

    _it += valueString.size();

    SkipWhitespace(_it, _end);
    assert(_it != _end);

    return valueString;
}

void InspectionContext::Struct(std::string _name, InspectionType _type, u32 _version)
{
    switch (m_Operation)
    {
        case Operation::ToText:
        {
            if (m_TextBuffer->empty())
            {
                assert(m_Depth == 0);

                // Just starting, emit the type identifier
                *m_TextBuffer += ToString(_type) + " v" + std::to_string(_version) + "\n{\n";
            }
            else
            {
                *m_TextBuffer += _name + ":\n{\n";
            }

            ++m_Depth;
            break;
        }
        case Operation::FromText:
        {
            if (m_Depth == 0)
            {
                {
                    const std::string expectedTypeIdentifier = ToString(_type);
                    assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, expectedTypeIdentifier));

                    m_TextIt += expectedTypeIdentifier.size();
                }

                SkipWhitespace(m_TextIt, m_TextEnd);
                assert(m_TextIt != m_TextEnd);

                {
                    const std::string expectedVersionIdentifier = "v" + std::to_string(_version);
                    assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, expectedVersionIdentifier));

                    m_TextIt += expectedVersionIdentifier.size();
                }
            }
            else
            {
                const std::string expectedPropertyIdentifier = _name + ":";
                assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, expectedPropertyIdentifier));

                m_TextIt += expectedPropertyIdentifier.size();
            }

            SkipWhitespace(m_TextIt, m_TextEnd);
            assert(m_TextIt != m_TextEnd);

            {
                assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, "{"));
                ++m_TextIt;
            }

            SkipWhitespace(m_TextIt, m_TextEnd);
            assert(m_TextIt != m_TextEnd);

            ++m_Depth;

            break;
        }
        default:
            break;
    }
}

void InspectionContext::EndStruct()
{
    switch (m_Operation)
    {
        case Operation::ToText:
        {
            assert(m_Depth != 0);
            *m_TextBuffer += "}\n";

            --m_Depth;

            break;
        }
        case Operation::FromText:
        {
            assert(m_Depth != 0);
            assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, "}"));
            ++m_TextIt;

            SkipWhitespace(m_TextIt, m_TextEnd);

            --m_Depth;

            if (m_Depth == 0)
            {
                assert(m_TextIt == m_TextEnd);
            }

            break;
        }
        default:
            break;
    }
}

void InspectionContext::U32(std::string _name, u32& _value)
{
    switch (m_Operation)
    {
        case Operation::ToText:
        {
            *m_TextBuffer += _name + ": " + std::to_string(_value) + "\n";

            break;
        }
        case Operation::FromText:
        {
            std::string valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

            _value = static_cast<u32>(atol(valueString.c_str()));

            break;
        }
        default:
            break;
    }
}

void InspectionContext::S32(std::string _name, s32& _value)
{
    switch (m_Operation)
    {
    case Operation::ToText:
    {
        *m_TextBuffer += _name + ": " + std::to_string(_value) + "\n";

        break;
    }
    case Operation::FromText:
    {
        std::string valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

        _value = static_cast<s32>(atoi(valueString.c_str()));

        break;
    }
    default:
        break;
    }
}

void InspectionContext::F32(std::string _name, f32& _value)
{
    switch (m_Operation)
    {
    case Operation::ToText:
    {
        *m_TextBuffer += _name + ": " + std::to_string(_value) + "\n";

        break;
    }
    case Operation::FromText:
    {
        std::string valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

        _value = static_cast<f32>(atof(valueString.c_str()));

        break;
    }
    default:
        break;
    }
}

void InspectionContext::Bool(std::string _name, bool& _value)
{
    switch (m_Operation)
    {
    case Operation::ToText:
    {
        *m_TextBuffer += _name + ": " + (_value ? "true" : "false") + "\n";

        break;
    }
    case Operation::FromText:
    {
        std::string valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

        _value = (valueString == "true");

        break;
    }
    default:
        break;
    }
}
