//
// Created by alastair on 07/10/17.
//

#pragma once

#include <mutex>
#include <vector>

#include <src/tools/globals.h>

class MessageLog
{
public:
    MessageLog(const MessageLog&) = delete;
    void operator=(const MessageLog&) = delete;

    static MessageLog& GetInstance();
    std::vector<std::pair<LogType, std::string>> GetMessageBuffer() const;

    void InsertMessage(LogType _type, std::string _message);

private:
    MessageLog();

private:
    std::vector<std::pair<LogType, std::string>> m_MessageBuffer;
    mutable std::mutex m_BufferAccessMutex;
};
