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
