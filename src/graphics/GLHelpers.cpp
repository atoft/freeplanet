//
// Created by alastair on 17/02/2020.
//

#include "GLHelpers.h"

// Produces a C++11 string from an OpenGL-style string.
std::string GLHelpers::ConvertGLString(const GLubyte* _glstring)
{
    return std::string(reinterpret_cast<const char*>(_glstring));
}

void GLHelpers::ReportProperties()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    if(glGetString(GL_VERSION) == nullptr)
    {
        Globals::logError("OpenGL error", "Failed to get OpenGL properties. Is there a valid OpenGL context?");
        return;
    }

    LogMessage("Version number: " + ConvertGLString(glGetString(GL_VERSION))
        + "\n\tProvided by: " + ConvertGLString(glGetString(GL_VENDOR))
        + "\n\tRunning on: " + ConvertGLString(glGetString(GL_RENDERER))
        + "\n\tPrimary GLSL version supported: " + ConvertGLString(glGetString(GL_SHADING_LANGUAGE_VERSION))
        + "\n\tPowered by: Sticky tape, rubber bands.");
}

void GLHelpers::ReportError(std::string _location)
{
    static GLenum lastGLError = 0;
    static std::string lastGLErrLocation;
    static bool didRepeat = false;

    //TODO: Disable in release builds
    GLenum glErr = glGetError();

    if(glErr != 0)
    {
        if(didRepeat == true && lastGLError == glErr && lastGLErrLocation == _location)
        {
            return;
        }
        if(lastGLError == glErr && lastGLErrLocation == _location)
        {
            didRepeat = true;
            LogError(ConvertGLString(gluErrorString(glErr)) + " at \"" + _location + "\" occurred again. Suppressing further errors...");
            return;
        }
        LogError(ConvertGLString(gluErrorString(glErr)) + " at \"" + _location + "\"" );
        lastGLError = glErr;
        lastGLErrLocation = _location;
        didRepeat = false;
    }
}
