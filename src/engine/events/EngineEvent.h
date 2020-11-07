//
// Created by alastair on 02/10/18.
//


#pragma once

#include <variant>
#include <optional>
#include <vector>

#include <src/tools/globals.h>

// Top-level events. These shouldn't be used within systems, eg. WorldObjects communicating with each other,
// they should be for handling events across systems. May split into finer grained event types if this becomes
// cumbersome or expensive.
class EngineEvent
{
public:
    enum class Type
    {
        Echo = 0,
        Help,
        UIRequestGameMenu,
        UIRequestDebugMenu,

        WorldRequestDestroyWorldObject,
        WorldSetCollision,
        WorldSetPlanetaryRotationSpeedScale,
        WorldSetTimeScale,
        WorldSetGravity,
        WorldFreezeZones,

        EngineRequestQuit,
        EngineTestWorld,
        EngineLoadPlanetFromSeed,

        RenderSetBoundingBoxes,
        RenderSetWireframe,
        RenderSetVista,
        RenderOnDynamicMeshDestroyed,
        RenderMaxTerrainLOD,
        RenderSetParticles,

        TerrainSetNormalGenerationMethod,

        InputSetMouseLock,
        
        OnQuit,

        Count
    };

    enum class ArgumentType
    {
        Void = 0,
        Int,
        Float,
        String,

        Count
    };
    struct CommandDefinition
    {
        Type m_EventType;
        bool m_IsAvailableToUser;
        std::string m_Name;
        std::string m_Description;
        ArgumentType  m_ArgumentType;
    };

public:
    EngineEvent(Type _type, std::string _data);
    EngineEvent(Type _type, u64 _data);
    EngineEvent(Type _type, f32 _data);
    explicit EngineEvent(Type _type);

    Type GetType() const { return m_Type; };

    std::string GetStringData() const;
    u64 GetIntData() const;
    f32 GetFloatData() const;

    static std::optional<EngineEvent> ParseEvent(std::string _input);
    static std::string GetHelpText();

private:
    Type m_Type;

    std::variant<u64, std::string, f32> m_Data;

    static bool IsIntType(Type _type);
    static bool IsStringType(Type _type);
    static bool IsFloatType(Type _type);
    static bool IsVoidType(Type _type);
};
