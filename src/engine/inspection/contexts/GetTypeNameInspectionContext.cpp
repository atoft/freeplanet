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

#include "GetTypeNameInspectionContext.h"

void GetTypeNameInspectionContext::Struct(std::string _name,
                                   InspectionType _type,
                                   u32 _version,
                                   InspectionStructRequirements _requirements)
{
    if (m_Result.empty())
    {
        m_Result = _name;
    }
}

void GetTypeNameInspectionContext::EndStruct()
{
}

void GetTypeNameInspectionContext::U32(std::string _name, u32& _value)
{
    if (m_Result.empty())
    {
        m_Result = "U32";
    }
}

void GetTypeNameInspectionContext::S32(std::string _name, s32& _value)
{
    if (m_Result.empty())
    {
        m_Result = "S32";
    }
}

void GetTypeNameInspectionContext::F32(std::string _name, f32& _value)
{
    if (m_Result.empty())
    {
        m_Result = "F32";
    }
}

void GetTypeNameInspectionContext::Bool(std::string _name, bool& _value)
{
    if (m_Result.empty())
    {
        m_Result = "Bool";
    }
}
