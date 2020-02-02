//
// Created by alastair on 04/05/17.
//

#include "StartupHelpers.h"

#include <string>

#include <src/tools/globals.h>

std::optional<CommandLineArgs> StartupHelpers::ParseCommandLine(int argc, char** argv)
{
    CommandLineArgs result;

    // Skip the first argument - program name by convention
    for(int arg = 1; arg < argc; ++arg)
    {
        const std::string currentArg = argv[arg];

        if(currentArg == "--forceUnlockedMouse")
        {
            result.m_ForceUnlockedMouse = true;
        }
        else if(currentArg == "--forceWindowed")
        {
            result.m_ForceWindowed = true;
        }
        else if(currentArg == "--useStdIn")
        {
            result.m_UseStandardInput = true;
        }
        else if(currentArg == "--unitTest")
        {
            result.m_UnitTest = true;
        }
        else if(currentArg == "--shaderDebug")
        {
            result.m_ShaderDebug = true;
        }
        else if(currentArg == "--triangleDebug")
        {
            result.m_TriangleDebug = true;
        }
        else if(currentArg == "--noclip")
        {
            result.m_Noclip = true;
        }
        else if (currentArg == "--testWorld")
        {
            if (arg + 1 >= argc)
            {
                LogError("Not enough arguments for --testWorld");
                return std::nullopt;
            }

            result.m_TestWorld = argv[arg + 1];

            // Skip the next argument, already handled.
            ++arg;
        }
        else if (currentArg == "--debugUI")
        {
            if (arg + 1 >= argc)
            {
                LogError("Not enough arguments for --debugUI");
                return std::nullopt;
            }

            result.m_DebugUI = argv[arg + 1];

            // Skip the next argument, already handled.
            ++arg;
        }
        else
        {
            LogError("Unexpected command line argument " + currentArg);
            return std::nullopt;
        }
    }
    return result;
}
