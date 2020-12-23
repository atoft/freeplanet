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
