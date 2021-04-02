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

#include "EngineEvent.h"

#include <algorithm>

#include <src/tools/StringHelpers.h>

std::vector<EngineEvent::CommandDefinition> EngineEvent::COMMAND_DEFINITIONS =
        {
            {EngineEvent::Type::Echo, true, "echo", "Print the specified string.", EngineEvent::ArgumentType::String},
            {EngineEvent::Type::Help, true, "help", "Display the help text.", EngineEvent::ArgumentType::Void},
            {EngineEvent::Type::UIRequestDebugMenu, true, "debugui", "Open the specified debug menu.", EngineEvent::ArgumentType::String},
            {EngineEvent::Type::WorldRequestDestroyWorldObject, true, "destroy", "Request to destroy the WorldObject with the given ID.", EngineEvent::ArgumentType::Int},
            {EngineEvent::Type::WorldSetCollision, true, "setcollision", "Set the world's collision mode (0,1).", EngineEvent::ArgumentType::Int},
            {EngineEvent::Type::WorldSetPlanetaryRotationSpeedScale, true, "sunspeed", "Scale the speed of the planet's rotation.", EngineEvent::ArgumentType::Float},
            {EngineEvent::Type::WorldSetTimeScale, true, "timescale", "Set the factor to scale gameplay speed by.", EngineEvent::ArgumentType::Float},
            {EngineEvent::Type::WorldSetGravity, true, "gravity", "Set the strength of gravity.", EngineEvent::ArgumentType::Float},
            {EngineEvent::Type::WorldFreezeZones, true, "freeze", "Prevent the active zones from changing (0,1).", EngineEvent::ArgumentType::Int},
            {EngineEvent::Type::EngineRequestQuit, true, "quit", "Request to end any running tasks and quit the engine.", EngineEvent::ArgumentType::Void},
            {EngineEvent::Type::EngineTestWorld, true, "testworld", "Switch world to the specified test world.", EngineEvent::ArgumentType::String},
            {EngineEvent::Type::EngineNewPlanet, true, "new", "Create a world with a planet based on [name seed] provided.", EngineEvent::ArgumentType::EnginePlanetRequest},
            {EngineEvent::Type::EngineLoadPlanet, true, "load", "Load the world with the given name.", EngineEvent::ArgumentType::String},
            {
                EngineEvent::Type::RenderSetBoundingBoxes,
                true,
                "bb",
                "Set the render mode for bounding boxes. None = 0, Components = 1, Zones = 2, Chunks = 3, Normals = 4.",
                EngineEvent::ArgumentType::Int
            },
            {EngineEvent::Type::RenderSetWireframe, true, "wireframe", "Set whether to render the world as wireframes (0,1).", EngineEvent::ArgumentType::Int},
            {EngineEvent::Type::RenderSetVista, true, "vista", "Set whether to render the vista (0,1).", EngineEvent::ArgumentType::Int},
            {EngineEvent::Type::RenderMaxTerrainLOD, true, "tlod", "Set the max terrain LOD for the shader.", EngineEvent::ArgumentType::Int},
            {EngineEvent::Type::RenderSetParticles, true, "particles", "Set whether to render particles (0,1)..", EngineEvent::ArgumentType::Int},
            
            {EngineEvent::Type::TerrainSetNormalGenerationMethod, true, "normals", "Set how to compute terrain normals. From face normals = 0, From volume = 1.", EngineEvent::ArgumentType::Int},

            {EngineEvent::Type::InputSetMouseLock, true, "mouse", "Toggle mouse lock. Mouse is free = 0, Mouse is controlled by the engine = 1.", EngineEvent::ArgumentType::Int},
            
            {EngineEvent::Type::OnQuit, false, "", "Fired when Engine has started quitting.", EngineEvent::ArgumentType::Void},
            {EngineEvent::Type::RenderOnDynamicMeshDestroyed, false, "", "Fired when a Dynamic Mesh is destroyed.", EngineEvent::ArgumentType::Int}

        };

constexpr u64 VOID_DATA = 0;

EngineEvent::EngineEvent(Type _type)
        : m_Type(_type),
          m_Data(VOID_DATA)
{
    assert(HasDataType<void>(_type));
}

std::string EngineEvent::GetStringData() const
{
    return Get<std::string>();
}

u64 EngineEvent::GetIntData() const
{
    return Get<u64>();
}

f32 EngineEvent::GetFloatData() const
{
    return Get<f32>();
}

/*static*/ std::optional<EngineEvent> EngineEvent::ParseEvent(std::string _input)
{
    std::vector<std::string> tokens = StringHelpers::Split(_input, " ");

    if(tokens.empty() || tokens.front().empty())
    {
        LogError("Parsing failed: no tokens found.");
        return std::nullopt;
    }

	std::string commandName = tokens.front();

    const auto& it = std::find_if(COMMAND_DEFINITIONS.begin(), COMMAND_DEFINITIONS.end(), [commandName](const EngineEvent::CommandDefinition& definition)
    {
        return definition.m_Name == commandName;
    });

    if(it != COMMAND_DEFINITIONS.end())
    {
        switch (it->m_ArgumentType)
        {
            case EngineEvent::ArgumentType::String:
            {
                if(tokens.size() < 2)
                {
                    LogError("Parsing failed: incorrect number of tokens for " + tokens.front());
                    return std::nullopt;
                }
                return EngineEvent(it->m_EventType, _input.substr(commandName.size() + 1, -1));
            }
            case EngineEvent::ArgumentType::Int:
            {
                if(tokens.size() != 2)
                {
                    LogError("Parsing failed: incorrect number of tokens for " + tokens.front());
                    return std::nullopt;
                }
                const u64 intData = atoi(tokens[1].data());
                return EngineEvent(it->m_EventType, intData);
            }
            case EngineEvent::ArgumentType::Float:
            {
                if(tokens.size() != 2)
                {
                    LogError("Parsing failed: incorrect number of tokens for " + tokens.front());
                    return std::nullopt;
                }
                const f32 floatData = atof(tokens[1].data());
                return EngineEvent(it->m_EventType, floatData);
            }
            case EngineEvent::ArgumentType::EnginePlanetRequest:
            {
                if(tokens.size() != 3)
                {
                    LogError("Parsing failed: incorrect number of tokens for " + tokens.front());
                    return std::nullopt;
                }
                EnginePlanetRequest request;
                request.m_Name = tokens[1];
                request.m_Seed = atoi(tokens[2].data());
                return EngineEvent(it->m_EventType, request);
            }
            case EngineEvent::ArgumentType::Void:
            default:
            {
                if(tokens.size() != 1)
                {
                    LogError("Parsing failed: incorrect number of tokens for " + tokens.front());
                    return std::nullopt;
                }
                return EngineEvent(it->m_EventType);
            }
        }
        static_assert(static_cast<u32>(EngineEvent::ArgumentType::Count) == 5);
    }
    else
    {
        LogError("Parsing failed: Token " + tokens.front() + " currently unsupported.");
        return std::nullopt;
    }
}

std::string EngineEvent::GetHelpText()
{
    std::string helpText = "This is the in-game console. Available commands:";
    for(const auto& command : COMMAND_DEFINITIONS)
    {
        if(!command.m_IsAvailableToUser)
        {
            continue;
        }

        helpText += "\n\t" + command.m_Name + " ";
        switch (command.m_ArgumentType)
        {
            case EngineEvent::ArgumentType::Int:
                helpText += "<integer>";
                break;
            case EngineEvent::ArgumentType::String:
                helpText += "<string>";
                break;
            case EngineEvent::ArgumentType::Float:
                helpText += "<float>";
                break;
            case EngineEvent::ArgumentType::EnginePlanetRequest:
                helpText += "<name seed>";
                break;
            case EngineEvent::ArgumentType::Void:
            default:
                break;
        }
        static_assert(static_cast<u32>(EngineEvent::ArgumentType::Count) == 5);

        helpText += "\n\t\t\t" + command.m_Description;
    }
    return helpText;
}
