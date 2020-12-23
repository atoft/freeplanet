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

#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <src/config.h>

enum class ThreadType
{
    Invalid = 0,
    Main,
    Render,
    Loader,
    Count
};

class ThreadUtils
{
public:
    static thread_local ThreadType tl_ThreadType;
};

// @CrossPlatform freeplanet prefix is a hack to avoid name clash with ERROR which is #defined to 0 on Windows.
enum class LogType
{
    freeplanetMESSAGE,
    freeplanetWARNING,
    freeplanetERROR
};

std::string StripFunctionString(std::string functionName);

// @CrossPlatform __PRETTY_FUNCTION__ is non-standard C++ but Clang supports it.
#define LogMessage(message) Globals::log( StripFunctionString(__PRETTY_FUNCTION__), message )
#define LogWarning(message) Globals::logWarning( StripFunctionString(__PRETTY_FUNCTION__), message )
#define LogError(message) Globals::logError( StripFunctionString(__PRETTY_FUNCTION__), message )

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s32 = std::int32_t;
using f32 = std::float_t;
using f64 = std::double_t;
using TimeMS = f32;
using TimeSeconds = f32;

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(s32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

namespace Globals
{
    void logError(std::string classname, std::string error);
    void logWarning(std::string classname, std::string warning);
    void log(std::string classname, std::string message);

    const std::string FREEPLANET_ASSET_PATH = "assets/";
    const f32 FREEPLANET_FAR_CLIP_DISTANCE = 100000.0f;
    const f32 FREEPLANET_FOV_DEGREES = 90.0f;
    const bool FREEPLANET_IS_GRAVITY = true;
};
