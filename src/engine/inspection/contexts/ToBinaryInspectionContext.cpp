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

#include "ToBinaryInspectionContext.h"
#include <cstring>

#include <src/engine/inspection/InspectionUtils.h>

void ToBinaryInspectionContext::Struct(std::string, InspectionType _type, u32 _version, InspectionStructRequirements)
{
    if (m_Buffer->empty())
    {
        // Write the type identifier - InspectionType and version each padded to u32 (for now).
        WriteU32(static_cast<u32>(_type));
        WriteU32(static_cast<u32>(_version));
    }
}

void ToBinaryInspectionContext::EndStruct()
{

}

void ToBinaryInspectionContext::U32(std::string _name, const u32& _value)
{
    WriteU32(_value);
}

void ToBinaryInspectionContext::S32(std::string _name, const s32& _value)
{
    WriteU32(InspectionUtils::RawCast<s32, u32>(_value));
}

void ToBinaryInspectionContext::F32(std::string _name, const f32& _value)
{
    WriteU32(InspectionUtils::RawCast<f32, u32>(_value));    
}

void ToBinaryInspectionContext::Bool(std::string _name, const bool& _value)
{
    // Could support packing of bools if there are multiple in sequence.
    m_Buffer->push_back(_value ? 0x1 : 0x0);
}

void ToBinaryInspectionContext::WriteU32(u32 _value)
{
    constexpr u32 bytesInU32 = sizeof(u32) / sizeof(u8);

    m_Buffer->reserve(m_Buffer->size() + bytesInU32);
    for (s32 idx = bytesInU32 - 1; idx >= 0; --idx)
    {
        const u8 byte = (_value >> (idx * CHAR_BIT)) & UCHAR_MAX;
        m_Buffer->push_back(byte);
    }
}

