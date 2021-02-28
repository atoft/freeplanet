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

#include "FromBinaryInspectionContext.h"
#include "src/engine/inspection/InspectionUtils.h"
#include "src/engine/inspection/InspectionTypes.h"
#include <cstring>

void FromBinaryInspectionContext::Struct(std::string, InspectionType _type, u32 _version, InspectionStructRequirements)
{
    ++m_Depth;

    if (m_Finished)
    {
        return;
    }
    
    if (m_Depth == 1)
    {
        const std::optional<u32> typeIdentifier = ReadU32();

        if (!typeIdentifier)
        {
            AddError("Read failed at type identifier");
            return;
        }

        if (*typeIdentifier != static_cast<u32>(_type))
        {
            AddError("Incorrect type identifier found (" + std::to_string(*typeIdentifier) + ")");
            return;
        }

        const std::optional<u32> version = ReadU32();

        if (!version)
        {
            AddError("Read failed at version number");
            return;
        }

        if (*version != _version)
        {
            AddError("Incorrect version (found " + std::to_string(*version) + ", expected " + std::to_string(_version) + ")");
            return;
        }
    }
}

void FromBinaryInspectionContext::EndStruct()
{
    assert(m_Depth > 0);
    --m_Depth;

    if (m_Depth == 0)
    {
        if (m_It != m_End)
        {
            AddError("Unexpected extra data in buffer");
        }
    }
}

void FromBinaryInspectionContext::U32(std::string _name, u32& _value)
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

    _value = *readValue;
}

void FromBinaryInspectionContext::S32(std::string _name, s32& _value)
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

    _value = InspectionUtils::RawCast<u32, s32>(*readValue);
}

void FromBinaryInspectionContext::F32(std::string _name, f32& _value)
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

    _value = InspectionUtils::RawCast<u32, f32>(*readValue);
}

void FromBinaryInspectionContext::Bool(std::string _name, bool& _value)
{
    if (m_Finished)
    {
        return;
    }

    if (m_It >= m_End)
    {
        AddError("The buffer is malformed, causing an attempted overflow");
        AddError("Read failed at " + _name);
        return;
    }

    const u8 readValue = *m_It;
    ++m_It;

    if (readValue == 0x1)
    {
        _value = true;
    }
    else if (readValue == 0x0)
    {
        _value = false;
    }
    else
    {
        AddError("The buffer contains a malformed bool");
    }
}

std::optional<u32> FromBinaryInspectionContext::ReadU32()
{
    constexpr u32 bytesInU32 = sizeof(u32) / sizeof(u8);

    if (m_It >= m_End || m_End - m_It < bytesInU32)
    {
        AddError("The buffer is malformed, causing an attempted overflow");
        return std::nullopt;
    }

    u32 value = 0;
    
    for (s32 idx = bytesInU32 - 1; idx >= 0; --idx)
    {
        value |= (*m_It << (idx * CHAR_BIT));
        ++m_It;
    }

    return value;
}

void FromBinaryInspectionContext::AddError(const std::string& _error)
{
    m_Finished = true;
    m_Result = FromBinaryInspectionResult::ReadSyntaxError;
    m_ErrorMessage += "Error: " + _error + ".""\n";
}

void FromBinaryInspectionContext::AddWarning(const std::string& _error)
{
    m_WarningMessage += "Warning: " + _error + ".""\n";
}

