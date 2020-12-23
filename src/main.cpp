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

#include <src/engine/PlatformHelpers.h>
#include <src/engine/Engine.h>
#include <src/engine/StartupHelpers.h>
#include <src/tests/RunTests.h>

int main(int argc, char** argv)
{
    const std::optional<CommandLineArgs> parsedArgs = StartupHelpers::ParseCommandLine(argc, argv);

    if (parsedArgs == std::nullopt)
    {
        return -1;
    }

    PlatformHelpers::PlatformInit();

    if (parsedArgs->m_UnitTest)
    {
        LogMessage("Running unit tests.");

        const bool testSuccess = Test::RunTests();
        return !testSuccess;
    }

    Engine& engine = Engine::GetInstance();

    return engine.Run(*parsedArgs);
}
