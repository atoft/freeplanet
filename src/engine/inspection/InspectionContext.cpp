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

bool InspectionContext::SkipSingleToken(std::string::const_iterator& _it, std::string::const_iterator _end, const std::string& _token, bool _allowFailure)
{
    if (!StringHelpers::StartsWith(_it, _end, _token))
    {
        if (!_allowFailure)
        {
            AddError("Expected token \"" + _token + "\"");
        }
        else
        {
            AddWarning("Skipping expected token \"" + _token + "\"");
        }

        return false;
    }

    _it += _token.size();
    return true;
}

void InspectionContext::AddError(std::string _error)
{
    m_Finished = true;
    m_Result = InspectionResult::ReadSyntaxError;
    m_ErrorMessage += "Error: " + _error + " at line " + std::to_string(GetLineNumber()) + ".""\n";
}

void InspectionContext::AddWarning(std::string _error)
{
    if (m_Result != InspectionResult::ReadSyntaxError)
    {
        m_Result = InspectionResult::ReadIncomplete;
    }

    m_WarningMessage += "Warning: " + _error + " at line " + std::to_string(GetLineNumber()) + ".""\n";
}


u32 InspectionContext::GetLineNumber() const
{
    u32 lineNumber = 1;
    for (auto it = m_TextBegin; it < m_TextIt; ++it)
    {
        if (*it == '\n')
        {
            ++lineNumber;
        }
    }
    return lineNumber;
}

std::optional<std::string> InspectionContext::ParseValueAndSkip(std::string _name, std::string::const_iterator& _it, std::string::const_iterator _end)
{
    if (!m_Stack.back().m_InsideContainer)
    {
        const std::string expectedPropertyIdentifier = _name + ":";
        const bool canSkipThisToken = m_Stack.back().m_StructRequirements == InspectionStructRequirements::AllowMissingValues;

        if (!SkipSingleToken(_it, _end, expectedPropertyIdentifier, canSkipThisToken)) return std::nullopt;
    }

    SkipWhitespace(_it, _end);
    if (_it == _end) return AddError("Expected value for " + _name), std::nullopt;

    std::string endOfValueString = ";";
    std::string::const_iterator endOfLine = StringHelpers::Find(_it, _end, endOfValueString);

    if (m_Stack.back().m_InsideContainer)
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

    if (_it == _end) return AddError("Expected " + endOfValueString), std::nullopt;

    std::string valueString = std::string(_it, endOfLine);

    if (!SkipSingleToken(_it, _end, valueString)) return std::nullopt;
    if (!SkipSingleToken(_it, _end, endOfValueString)) return std::nullopt;

    SkipWhitespace(_it, _end);
    if (_it == _end) return AddError("Unexpected end of file"), std::nullopt;

    return valueString;
}

void InspectionContext::AppendNameAndValue(std::string _name, std::string _value)
{
    if (!m_Stack.back().m_InsideContainer)
    {
        AppendNewlineWithIndent();
        *m_TextBuffer += _name + ": ";
    }

    *m_TextBuffer += _value;

    if (!m_Stack.back().m_InsideContainer)
    {
        *m_TextBuffer += ";";
    }
}

void InspectionContext::AppendNewlineWithIndent()
{
    *m_TextBuffer += "\n";

    for (u32 idx = 0; idx < m_Stack.size(); ++idx)
    {
        *m_TextBuffer += "\t";
    }
}

void InspectionContext::Struct(std::string _name, InspectionType _type, u32 _version, InspectionStructRequirements _requirements)
{
    if (m_Finished || (!m_Stack.empty() && m_Stack.back().m_SkipThisLevel))
    {
        return;
    }

    switch (m_Operation)
    {
        case Operation::ToText:
        {
            if (m_TextBuffer->empty())
            {
                assert(m_Stack.empty());

                // Just starting, emit the type identifier
                *m_TextBuffer += InspectionTypeToString(_type) + " v" + std::to_string(_version) + "\n{";
            }
            else
            {
                if (!m_Stack.back().m_InsideContainer)
                {
                    AppendNewlineWithIndent();
                    *m_TextBuffer += _name + ":";
                }

                AppendNewlineWithIndent();
                *m_TextBuffer += "{";
            }

            m_Stack.push_back({false, _requirements});

            break;
        }
        case Operation::FromText:
        {
            if (m_Stack.empty())
            {
                {
                    const std::string expectedTypeIdentifier = InspectionTypeToString(_type);
                    if (!SkipSingleToken(m_TextIt, m_TextEnd, expectedTypeIdentifier)) return;
                }

                SkipWhitespace(m_TextIt, m_TextEnd);
                assert(m_TextIt != m_TextEnd);

                {
                    const std::string expectedVersionIdentifier = "v" + std::to_string(_version);
                    if (!SkipSingleToken(m_TextIt, m_TextEnd, expectedVersionIdentifier)) return;
                }
            }
            else
            {
                if (!m_Stack.back().m_InsideContainer)
                {
                    const std::string expectedPropertyIdentifier = _name + ":";
                    const bool canSkipThisToken = m_Stack.back().m_StructRequirements == InspectionStructRequirements::AllowMissingValues;
                    if (!SkipSingleToken(m_TextIt, m_TextEnd, expectedPropertyIdentifier, canSkipThisToken))
                    {
                        if (canSkipThisToken)
                        {
                            m_Stack.push_back({false, _requirements, true});
                        }

                        return;
                    }
                }
            }

            SkipWhitespace(m_TextIt, m_TextEnd);
            assert(m_TextIt != m_TextEnd);

            if (!SkipSingleToken(m_TextIt, m_TextEnd, "{")) return;

            SkipWhitespace(m_TextIt, m_TextEnd);
            assert(m_TextIt != m_TextEnd);

            m_Stack.push_back({false, _requirements});

            break;
        }
        default:
            break;
    }
}

void InspectionContext::EndStruct()
{
    if (m_Finished)
    {
        return;
    }

    switch (m_Operation)
    {
        case Operation::ToText:
        {
            assert(!m_Stack.empty());

            m_Stack.pop_back();
            AppendNewlineWithIndent();

            *m_TextBuffer += "}";

            if (m_Stack.empty() || !m_Stack.back().m_InsideContainer)
            {
                *m_TextBuffer += ";";
            }

            break;
        }
        case Operation::FromText:
        {
            assert(!m_Stack.empty());

            if (m_Stack.back().m_SkipThisLevel)
            {
                m_Stack.pop_back();
                SkipWhitespace(m_TextIt, m_TextEnd);
                return;
            }

            if (!SkipSingleToken(m_TextIt, m_TextEnd, "}")) return;
            SkipWhitespace(m_TextIt, m_TextEnd);

            m_Stack.pop_back();

            if (m_Stack.empty())
            {
                if (!SkipSingleToken(m_TextIt, m_TextEnd, ";")) return;
                SkipWhitespace(m_TextIt, m_TextEnd);
                assert(m_TextIt == m_TextEnd);
            }
            else if (m_Stack.back().m_InsideContainer && !StringHelpers::StartsWith(m_TextIt, m_TextEnd, "]"))
            {
                if (!SkipSingleToken(m_TextIt, m_TextEnd, ",")) return;
            }
            else if (m_Stack.back().m_InsideContainer && StringHelpers::StartsWith(m_TextIt, m_TextEnd, "]"))
            {
                if (!SkipSingleToken(m_TextIt, m_TextEnd, "]")) return;
            }
            else
            {
                if (!SkipSingleToken(m_TextIt, m_TextEnd, ";")) return;
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
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    switch (m_Operation)
    {
        case Operation::ToText:
        {
            AppendNameAndValue(_name, std::to_string(_value));

            break;
        }
        case Operation::FromText:
        {
            const std::optional<std::string> valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

            if (!valueString)
            {
                return;
            }

            _value = static_cast<u32>(atol(valueString->c_str()));

            break;
        }
        default:
            break;
    }
}

void InspectionContext::S32(std::string _name, s32& _value)
{
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    switch (m_Operation)
    {
    case Operation::ToText:
    {
        AppendNameAndValue(_name, std::to_string(_value));

        break;
    }
    case Operation::FromText:
    {
        const std::optional<std::string> valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

        if (!valueString)
        {
            return;
        }

        _value = static_cast<s32>(atoi(valueString->c_str()));

        break;
    }
    default:
        break;
    }
}

void InspectionContext::F32(std::string _name, f32& _value)
{
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    switch (m_Operation)
    {
    case Operation::ToText:
    {
        AppendNameAndValue(_name, std::to_string(_value));

        break;
    }
    case Operation::FromText:
    {
        const std::optional<std::string> valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

        if (!valueString)
        {
            return;
        }
        _value = static_cast<f32>(atof(valueString->c_str()));

        break;
    }
    default:
        break;
    }
}

void InspectionContext::Bool(std::string _name, bool& _value)
{
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    switch (m_Operation)
    {
    case Operation::ToText:
    {
        AppendNameAndValue(_name, _value ? "true" : "false");

        break;
    }
    case Operation::FromText:
    {
        const std::optional<std::string> valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

        if (!valueString)
        {
            return;
        }

        _value = (*valueString == "true");

        break;
    }
    default:
        break;
    }
}
