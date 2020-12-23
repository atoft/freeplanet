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
