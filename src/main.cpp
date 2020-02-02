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
