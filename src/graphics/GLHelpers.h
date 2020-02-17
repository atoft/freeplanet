//
// Created by alastair on 17/02/2020.
//

#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <src/tools/globals.h>

class GLHelpers
{
public:
    static void ReportProperties();
    static void ReportError(std::string _location);

private:
    static std::string ConvertGLString(const GLubyte* _glstring);
};
