//
// Created by alastair on 04/05/17.
//

#pragma once

#include <string>
#include <optional>

struct CommandLineArgs
{
    bool m_ForceUnlockedMouse = false;
    bool m_ForceWindowed = false;
    bool m_UseStandardInput = false;
    bool m_UnitTest = false;
    bool m_ShaderDebug = false;
    bool m_TriangleDebug = false;
    bool m_Noclip = false;
    std::string m_TestWorld;
    std::string m_DebugUI;
};

namespace StartupHelpers
{
    std::optional<CommandLineArgs> ParseCommandLine(int argc, char** argv);
};
