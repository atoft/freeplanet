//
// Created by alastair on 19/01/2020.
//

#include "RunTests.h"

#include <src/tests/TestCollision.h>
#include <src/tests/TestInspection.h>

bool Test::RunTests()
{
    bool result = true;

    result &= TestCollision();
    result &= TestInspection();

    return result;
}