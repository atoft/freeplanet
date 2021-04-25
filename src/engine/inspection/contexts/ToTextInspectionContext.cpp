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

#include "ToTextInspectionContext.h"

void ToTextInspectionContext::AppendNameAndValue(std::string _name, std::string _value)
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

void ToTextInspectionContext::AppendNewlineWithIndent()
{
    *m_TextBuffer += "\n";

    for (u32 idx = 0; idx < m_Stack.size(); ++idx)
    {
        *m_TextBuffer += "\t";
    }
}

void ToTextInspectionContext::Struct(std::string _name,
                                   InspectionType _type,
                                   u32 _version,
                                   InspectionStructRequirements _requirements)
{
    if (m_TextBuffer->empty())
    {
        assert(m_Stack.empty());

        // Just starting, emit the type identifier
        *m_TextBuffer += _name + " v" + std::to_string(_version) + "\n{";
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

    m_Stack.push_back({false});
}

void ToTextInspectionContext::EndStruct()
{
    if (m_Finished)
    {
        return;
    }

    assert(!m_Stack.empty());

    m_Stack.pop_back();
    AppendNewlineWithIndent();

    *m_TextBuffer += "}";

    if (m_Stack.empty() || !m_Stack.back().m_InsideContainer)
    {
        *m_TextBuffer += ";";
    }
}

void ToTextInspectionContext::U32(std::string _name, u32& _value)
{
    AppendNameAndValue(_name, std::to_string(_value));
}

void ToTextInspectionContext::S32(std::string _name, s32& _value)
{
    AppendNameAndValue(_name, std::to_string(_value));
}

void ToTextInspectionContext::F32(std::string _name, f32& _value)
{
    AppendNameAndValue(_name, std::to_string(_value));
}

void ToTextInspectionContext::Bool(std::string _name, bool& _value)
{
    AppendNameAndValue(_name, _value ? "true" : "false");
}
