
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

void Globals::ReportGLError(std::string location)
{
    static GLenum lastGLError = 0;
    static std::string lastGLErrLocation = "";
    static bool didRepeat = false;

    //TODO: Disable in release builds
    GLenum glErr = glGetError();

    if(glErr != 0)
    {
        if(didRepeat == true && lastGLError == glErr && lastGLErrLocation == location)
        {
            return;
        }
        if(lastGLError == glErr && lastGLErrLocation == location)
        {
            didRepeat = true;
            Globals::logError("OpenGL error", ConvertGLString(gluErrorString(glErr)) + " at \"" + location + "\" occurred again. Suppressing further errors...");
            return;
        }
        Globals::logError("OpenGL error", ConvertGLString(gluErrorString(glErr)) + " at \"" + location + "\"" );
        lastGLError = glErr;
        lastGLErrLocation = location;
        didRepeat = false;
    }

}

// Produces a C++11 string from an OpenGL-style string.
std::string Globals::ConvertGLString(const GLubyte *glstring)
{
    return std::string(reinterpret_cast<const char*>(glstring));
}

void Globals::ReportGLProperties()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    if(glGetString(GL_VERSION) == nullptr)
    {
        Globals::logError("OpenGL error", "Failed to get OpenGL properties. Is there a valid OpenGL context?");
        return;
    }

    Globals::log( "OpenGL message", "Version number: " + ConvertGLString(glGetString(GL_VERSION))
              + "\n\tProvided by: " + ConvertGLString(glGetString(GL_VENDOR))
              + "\n\tRunning on: " + ConvertGLString(glGetString(GL_RENDERER))
              + "\n\tPrimary GLSL version supported: " + ConvertGLString(glGetString(GL_SHADING_LANGUAGE_VERSION))
              + "\n\tPowered by: Sticky tape, rubber bands.");
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

void Globals::logVerbose(std::string classname, std::string message)
{
    if(Globals::FREEPLANET_PRINT_VERBOSE_LOGS)
    {
        std::string outMessage = "[" + classname + "] " + message;
        std::cout << outMessage << std::endl;
        MessageLog::GetInstance().InsertMessage(LogType::freeplanetMESSAGE, outMessage);
    }
}
