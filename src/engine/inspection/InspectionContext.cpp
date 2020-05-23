//
// Created by alastair on 17/01/2020.
//

#include "InspectionContext.h"

std::vector<TypeInfo> InspectionContext::ms_TypeInfos =
        {
            {"vec2", InspectionType::vec2},
            {"vec3", InspectionType::vec3},

            {"ivec2", InspectionType::ivec2},
            {"ivec3", InspectionType::ivec3},

            {"TestStruct", InspectionType::TestStruct},
            {"TestSubStruct", InspectionType::TestSubStruct},
            {"TestStructInVector", InspectionType::TestStructInVector},

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

std::string InspectionContext::InspectionTypeToString(InspectionType _type)
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

void InspectionContext::SkipWhitespace(std::string::const_iterator& _it, std::string::const_iterator _end)
{
    for (; _it != _end; ++_it)
    {
        if (*_it != ' ' && *_it != '\n' && *_it != '\t')
        {
            break;
        }
    }
}

void InspectionContext::SkipSingleToken(std::string::const_iterator& _it, std::string::const_iterator _end, const std::string& _token)
{
    assert(StringHelpers::StartsWith(_it, _end, _token));

    _it += _token.size();
}

std::string InspectionContext::ParseValueAndSkip(std::string _name, std::string::const_iterator& _it, std::string::const_iterator _end)
{
    if (!m_InsideContainer.back())
    {
        const std::string expectedPropertyIdentifier = _name + ":";
        assert(StringHelpers::StartsWith(_it, _end, expectedPropertyIdentifier));
        _it += expectedPropertyIdentifier.size();
    }

    SkipWhitespace(_it, _end);
    assert(_it != _end);

    std::string endOfValueString = ";";
    std::string::const_iterator endOfLine = StringHelpers::Find(_it, _end, endOfValueString);

    if (m_InsideContainer.back())
    {
        // Find the closest token of , or ].
        const std::string::const_iterator nextComma = StringHelpers::Find(_it, _end, ",");
        const std::string::const_iterator nextClosingSquareBrace = StringHelpers::Find(_it, _end, "]");

        if (nextComma < nextClosingSquareBrace)
        {
            endOfValueString = ",";
            endOfLine = nextComma;
        }
        else
        {
            endOfValueString = "]";
            endOfLine = nextClosingSquareBrace;
        }
    }

    assert(endOfLine != _end);

    std::string valueString = std::string(_it, endOfLine);

    SkipSingleToken(_it, _end, valueString);
    SkipSingleToken(_it, _end, endOfValueString);

    SkipWhitespace(_it, _end);
    assert(_it != _end);

    return valueString;
}

void InspectionContext::AppendNameAndValue(std::string _name, std::string _value)
{
    if (!m_InsideContainer.back())
    {
        AppendNewlineWithIndent();
        *m_TextBuffer += _name + ": ";
    }

    *m_TextBuffer += _value;

    if (!m_InsideContainer.back())
    {
        *m_TextBuffer += ";";
    }
}

void InspectionContext::AppendNewlineWithIndent()
{
    *m_TextBuffer += "\n";

    for (u32 idx = 0; idx < m_Depth; ++idx)
    {
        *m_TextBuffer += "\t";
    }
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
                *m_TextBuffer += InspectionTypeToString(_type) + " v" + std::to_string(_version) + "\n{";
            }
            else
            {
                if (!m_InsideContainer.back())
                {
                    AppendNewlineWithIndent();
                    *m_TextBuffer += _name + ":";
                }

                AppendNewlineWithIndent();
                *m_TextBuffer += "{";
            }

            ++m_Depth;

            m_InsideContainer.push_back(false);
            break;
        }
        case Operation::FromText:
        {
            if (m_Depth == 0)
            {
                {
                    const std::string expectedTypeIdentifier = InspectionTypeToString(_type);
                    SkipSingleToken(m_TextIt, m_TextEnd, expectedTypeIdentifier);
                }

                SkipWhitespace(m_TextIt, m_TextEnd);
                assert(m_TextIt != m_TextEnd);

                {
                    const std::string expectedVersionIdentifier = "v" + std::to_string(_version);
                    SkipSingleToken(m_TextIt, m_TextEnd, expectedVersionIdentifier);
                }
            }
            else
            {
                if (!m_InsideContainer.back())
                {
                    const std::string expectedPropertyIdentifier = _name + ":";
                    SkipSingleToken(m_TextIt, m_TextEnd, expectedPropertyIdentifier);
                }
            }

            SkipWhitespace(m_TextIt, m_TextEnd);
            assert(m_TextIt != m_TextEnd);

            SkipSingleToken(m_TextIt, m_TextEnd, "{");

            SkipWhitespace(m_TextIt, m_TextEnd);
            assert(m_TextIt != m_TextEnd);

            ++m_Depth;
            m_InsideContainer.push_back(false);

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

            --m_Depth;
            AppendNewlineWithIndent();

            *m_TextBuffer += "}";

            m_InsideContainer.pop_back();

            if (!m_InsideContainer.back())
            {
                *m_TextBuffer += ";";
            }

            break;
        }
        case Operation::FromText:
        {
            assert(m_Depth != 0);
            SkipSingleToken(m_TextIt, m_TextEnd, "}");
            SkipWhitespace(m_TextIt, m_TextEnd);

            --m_Depth;
            m_InsideContainer.pop_back();

            if (m_Depth == 0)
            {
                SkipSingleToken(m_TextIt, m_TextEnd, ";");
                SkipWhitespace(m_TextIt, m_TextEnd);
                assert(m_TextIt == m_TextEnd);
            }
            else if (m_InsideContainer.back() && !StringHelpers::StartsWith(m_TextIt, m_TextEnd, "]"))
            {
                SkipSingleToken(m_TextIt, m_TextEnd, ",");
            }
            else if (m_InsideContainer.back() && StringHelpers::StartsWith(m_TextIt, m_TextEnd, "]"))
            {
                SkipSingleToken(m_TextIt, m_TextEnd, "]");
            }
            else
            {
                SkipSingleToken(m_TextIt, m_TextEnd, ";");
            }
            SkipWhitespace(m_TextIt, m_TextEnd);
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
            AppendNameAndValue(_name, std::to_string(_value));

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
        AppendNameAndValue(_name, std::to_string(_value));

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
        AppendNameAndValue(_name, std::to_string(_value));

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
        AppendNameAndValue(_name, _value ? "true" : "false");

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
