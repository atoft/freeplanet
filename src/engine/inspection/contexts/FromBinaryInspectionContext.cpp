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
#include <cstring>

void FromBinaryInspectionContext::Struct(std::string, InspectionType _type, u32 _version, InspectionStructRequirements)
{
    assert(!m_Finished);

    ++m_Depth;
    
    const std::optional<u32> typeIdentifier = ReadU32();

    if (!typeIdentifier)
    {
        m_Finished = true;
        return;
    }

    if (*typeIdentifier != static_cast<u32>(_type))
    {
        LogError("Incorrect type identifier found (" + std::to_string(*typeIdentifier) + ")");
        m_Finished = true;
        return;
    }

    const std::optional<u32> version = ReadU32();

    if (!version)
    {
        m_Finished = true;
        return;
    }

    if (*version != _version)
    {
        LogError("Incorrect version.");
        m_Finished = true;
        return;
    }
}

void FromBinaryInspectionContext::EndStruct()
{
    assert(m_Depth > 0);
    --m_Depth;
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
        m_Finished = true;
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
        m_Finished = true;
        return;
    }

    const u32 unsignedValue = *readValue;

    std::memcpy(&_value, &unsignedValue, sizeof(s32));
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
        m_Finished = true;
        return;
    }

    const u32 unsignedValue = *readValue;

    std::memcpy(&_value, &unsignedValue, sizeof(f32));
}

void FromBinaryInspectionContext::Bool(std::string _name, bool& _value)
{
    if (m_Finished)
    {
        return;
    }

    if (m_It >= m_End)
    {
        LogError("Attempted to read past end of buffer.");
        m_Finished = true;
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
        LogError("Malformed bool in buffer.");
        m_Finished = true;
    }
}

std::optional<u32> FromBinaryInspectionContext::ReadU32()
{
    constexpr u32 bytesInU32 = sizeof(u32) / sizeof(u8);

    if (m_It + bytesInU32 > m_End)
    {
        LogError("Attempted to read past end of buffer.");
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

