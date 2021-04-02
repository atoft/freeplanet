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
#include <algorithm>
#include "EnginePlanetRequest.h"

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
        EngineNewPlanet,
        EngineLoadPlanet,

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

        EnginePlanetRequest,

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
    template <typename DataType>
    EngineEvent(Type _type, DataType _data);

    explicit EngineEvent(Type _type);

    Type GetType() const { return m_Type; };

    std::string GetStringData() const;
    u64 GetIntData() const;
    f32 GetFloatData() const;

    template<typename DataType>
    DataType Get() const;

    static std::optional<EngineEvent> ParseEvent(std::string _input);
    static std::string GetHelpText();
    static std::vector<EngineEvent::CommandDefinition> COMMAND_DEFINITIONS;

private:
    Type m_Type;

    std::variant<u64, std::string, f32, EnginePlanetRequest> m_Data;

    template <typename DataType>
    static bool HasDataType(Type _type);
};

template <typename DataType>
EngineEvent::EngineEvent(Type _type, DataType _data)
        : m_Type(_type),
          m_Data(_data)
{
    assert(HasDataType<DataType>(_type));
}

template <typename DataType>
bool EngineEvent::HasDataType(Type _type)
{
    ArgumentType wantedArgType;

    if constexpr (std::is_same<DataType, void>())
    {
        wantedArgType = ArgumentType::Void;
    }
    else if constexpr (std::is_same<DataType, u64>())
    {
        wantedArgType = ArgumentType::Int;
    }
    else if constexpr (std::is_same<DataType, std::string>())
    {
        wantedArgType = ArgumentType::String;
    }
    else if constexpr (std::is_same<DataType, f32>())
    {
        wantedArgType = ArgumentType::Float;
    }
    else //if constexpr (std::is_same<DataType, EnginePlanetRequest>())
    {
        wantedArgType = ArgumentType::EnginePlanetRequest;
    }

    const auto& it = std::find_if(COMMAND_DEFINITIONS.begin(), COMMAND_DEFINITIONS.end(), [_type](const EngineEvent::CommandDefinition& definition)
    {
        return _type == definition.m_EventType;
    });

    return it != COMMAND_DEFINITIONS.end() && it->m_ArgumentType == wantedArgType;
}

template<typename DataType>
DataType EngineEvent::Get() const
{
    assert(HasDataType<DataType>(m_Type));

    return std::get<DataType>(m_Data);
}
