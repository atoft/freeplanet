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

#include "MessageLog.h"

MessageLog& MessageLog::GetInstance()
{
    static MessageLog instance;

    return instance;
}

MessageLog::MessageLog()
{
    auto lock = std::lock_guard(m_BufferAccessMutex);
    m_MessageBuffer = std::vector<std::pair<LogType,std::string>>();
}

std::vector<std::pair<LogType, std::string>> MessageLog::GetMessageBuffer() const
{
    auto lock = std::lock_guard(m_BufferAccessMutex);
    return m_MessageBuffer;
}

void MessageLog::InsertMessage(LogType _type, std::string _message)
{
    auto lock = std::lock_guard(m_BufferAccessMutex);
    constexpr u32 BUFFER_MAX_CAPACITY = 64;

    // TODO Would be improved using a circular buffer

    std::vector<std::pair<LogType,std::string>>& buffer = m_MessageBuffer;
    if(buffer.size() == BUFFER_MAX_CAPACITY)
    {
        // TODO flush the log to file
        buffer.erase(buffer.begin());
    }
    m_MessageBuffer.push_back(std::pair<LogType, std::string>(_type,_message));
}
