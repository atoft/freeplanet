//
// Created by alastair on 25/03/17.
//

#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
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
#define LogMessage(message) Globals::log( StripFunctionString(__PRETTY_FUNCTION__), message );
#define LogWarning(message) Globals::logWarning( StripFunctionString(__PRETTY_FUNCTION__), message );
#define LogError(message) Globals::logError( StripFunctionString(__PRETTY_FUNCTION__), message );


enum class GraphicsPrimitiveType
{
    QUAD,
    CUBE
};

enum class CollisionPrimitiveType
{
    NONE,
    AABB,
    OBB,
    TRIANGLE,
    TERRAIN,
    CAPSULE,
    SPHERE
};

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

using LocalComponentRef = u32;
using LocalWorldObjectRef = u32;

constexpr LocalWorldObjectRef REF_INVALID = std::numeric_limits<LocalWorldObjectRef>::max();

struct WorldObjectRef
{
    glm::ivec3 m_ZoneCoordinates;
    LocalWorldObjectRef m_LocalRef;

    bool operator==(const WorldObjectRef& _other) const
    {
        return m_ZoneCoordinates == _other.m_ZoneCoordinates 
            && m_LocalRef == _other.m_LocalRef;
    }

    WorldObjectRef(){};
    WorldObjectRef(glm::ivec3 _zoneCoords, LocalWorldObjectRef _localRef) : m_ZoneCoordinates(_zoneCoords), m_LocalRef(_localRef){};

    bool IsValid() const { return m_LocalRef != REF_INVALID; };
};

struct ComponentRef
{
    glm::ivec3 m_ZoneCoordinates;
    LocalComponentRef m_LocalRef;

    bool operator==(const WorldObjectRef& _other)
    {
        return m_ZoneCoordinates == _other.m_ZoneCoordinates 
            && m_LocalRef == _other.m_LocalRef;
    }
};

namespace Globals
{
    void ReportGLError(std::string location);
    void logError(std::string classname, std::string error);
    void logWarning(std::string classname, std::string warning);
    void log(std::string classname, std::string message);
    void logVerbose(std::string classname, std::string message);

    const int FREEPLANET_WINDOW_WIDTH = 1920;
    const int FREEPLANET_WINDOW_HEIGHT = 1080;
    const std::string FREEPLANET_ASSET_PATH = "assets/";
    const bool FREEPLANET_PRINT_VERBOSE_LOGS = true;
    const float FREEPLANET_FAR_CLIP_DISTANCE = 100000.0f;
    const float FREEPLANET_FOV_DEGREES = 90.0f;
    const bool FREEPLANET_IS_GRAVITY = true;

    void ReportGLProperties();

    void PrintCharArrayAsNumbers(unsigned char *array, int size);

    std::string ConvertGLString(const GLubyte *glstring);

};
