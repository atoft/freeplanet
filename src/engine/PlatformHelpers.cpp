//
// Created by alastair on 04/11/18.
//

#include "PlatformHelpers.h"

#include <src/tools/globals.h>

#ifdef freeplanet_LINUX
#include <X11/Xlib.h>
#endif

void PlatformHelpers::PlatformInit_Linux()
{
#ifdef freeplanet_LINUX
    LogMessage("Platform is Linux");
    XInitThreads();
#endif
}

void PlatformHelpers::PlatformInit_Windows()
{
#ifdef freeplanet_WIN32
    LogMessage("Platform is Windows");
#endif
}

void PlatformHelpers::PlatformInit()
{
    PlatformInit_Linux();
    PlatformInit_Windows();
}
