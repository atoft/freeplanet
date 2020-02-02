//
// Created by alastair on 04/11/18.
//

#pragma once

#include <src/config.h>

class PlatformHelpers
{
public:
    static void PlatformInit();

private:
    static void PlatformInit_Linux();
    static void PlatformInit_Windows();
};
