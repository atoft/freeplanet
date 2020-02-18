
#include <glm/glm.hpp>
#include <iostream>

#include "globals.h"

#include <src/tools/MessageLog.h>

thread_local ThreadType ThreadUtils::tl_ThreadType = ThreadType::Invalid;

// TODO we should be able to do this at compile time
std::string StripFunctionString(std::string functionName)
{
    std::string out = functionName.substr(0, functionName.find("(")); 
    return out.substr(out.find_last_of(" ")+1);
}

void Globals::logError(std::string classname, std::string error)
{
    std::string outMessage = "!Error [" + classname + "] " + error;
    std::cerr << outMessage << std::endl;
    MessageLog::GetInstance().InsertMessage(LogType::freeplanetERROR, outMessage);
}

void ::Globals::logWarning(std::string classname, std::string warning)
{
    std::string outMessage = "!Warning [" + classname + "] " + warning;
    std::cerr << outMessage << std::endl;
    MessageLog::GetInstance().InsertMessage(LogType::freeplanetWARNING, outMessage);
}

void Globals::log(std::string classname, std::string message)
{
    std::string outMessage = "[" + classname + "] " + message;
    std::cout << outMessage << std::endl;
    MessageLog::GetInstance().InsertMessage(LogType::freeplanetMESSAGE, outMessage);
}
