//
// Created by alastair on 07/10/17.
//

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
