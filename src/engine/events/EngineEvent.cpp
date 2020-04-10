//
// Created by alastair on 02/10/18.
//

#include "EngineEvent.h"

#include <algorithm>

#include <src/tools/StringHelpers.h>

const std::vector<EngineEvent::CommandDefinition> COMMAND_DEFINITIONS =
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
            {EngineEvent::Type::EngineLoadPlanetFromSeed, true, "seed", "Generate a world with a planet based on the seed provided.", EngineEvent::ArgumentType::Int},
            {
                EngineEvent::Type::RenderSetBoundingBoxes,
                true,
                "bb",
                "Set the render mode for bounding boxes. None = 0, Components = 1, Zones = 2, Chunks = 3, Normals = 4.",
                EngineEvent::ArgumentType::Int
            },
            {EngineEvent::Type::RenderSetWireframe, true, "wireframe", "Set whether to render the world as wireframes (0,1).", EngineEvent::ArgumentType::Int},
            {EngineEvent::Type::RenderSetVista, true, "vista", "Set whether to render the vista (0,1).", EngineEvent::ArgumentType::Int},

            {EngineEvent::Type::TerrainSetNormalGenerationMethod, true, "normals", "Set how to compute terrain normals. From face normals = 0, From volume = 1.", EngineEvent::ArgumentType::Int},

            {EngineEvent::Type::OnQuit, false, "", "Fired when Engine has started quitting.", EngineEvent::ArgumentType::Void},
            {EngineEvent::Type::RenderOnDynamicMeshDestroyed, false, "", "Fired when a Dynamic Mesh is destroyed.", EngineEvent::ArgumentType::Int}

        };

constexpr u64 VOID_DATA = 0;

bool EngineEvent::IsIntType(Type _type)
{
    const auto& it = std::find_if(COMMAND_DEFINITIONS.begin(), COMMAND_DEFINITIONS.end(), [_type](const EngineEvent::CommandDefinition& definition)
    {
        return _type == definition.m_EventType;
    });

    return it != COMMAND_DEFINITIONS.end() && it->m_ArgumentType == EngineEvent::ArgumentType::Int;
}

bool EngineEvent::IsFloatType(Type _type)
{
    const auto& it = std::find_if(COMMAND_DEFINITIONS.begin(), COMMAND_DEFINITIONS.end(), [_type](const EngineEvent::CommandDefinition& definition)
    {
        return _type == definition.m_EventType;
    });

    return it != COMMAND_DEFINITIONS.end() && it->m_ArgumentType == EngineEvent::ArgumentType::Float;
}

bool EngineEvent::IsStringType(Type _type)
{
    const auto& it = std::find_if(COMMAND_DEFINITIONS.begin(), COMMAND_DEFINITIONS.end(), [_type](const EngineEvent::CommandDefinition& definition)
    {
        return _type == definition.m_EventType;
    });

    return it != COMMAND_DEFINITIONS.end() && it->m_ArgumentType == EngineEvent::ArgumentType::String;
}

bool EngineEvent::IsVoidType(Type _type)
{
    const auto& it = std::find_if(COMMAND_DEFINITIONS.begin(), COMMAND_DEFINITIONS.end(), [_type](const EngineEvent::CommandDefinition& definition)
    {
        return _type == definition.m_EventType;
    });

    return it != COMMAND_DEFINITIONS.end() && it->m_ArgumentType == EngineEvent::ArgumentType::Void;
}

EngineEvent::EngineEvent(Type _type, std::string _data)
    : m_Type(_type),
      m_Data(_data)
{
    assert(IsStringType(_type));
}

EngineEvent::EngineEvent(Type _type, u64 _data)
    : m_Type(_type),
      m_Data(_data)
{
    assert(IsIntType(_type));
}

EngineEvent::EngineEvent(Type _type, f32 _data)
        : m_Type(_type),
          m_Data(_data)
{
    assert(IsFloatType(_type));
}

EngineEvent::EngineEvent(Type _type)
        : m_Type(_type),
          m_Data(VOID_DATA)
{
    assert(IsVoidType(_type));
}

std::string EngineEvent::GetStringData() const
{
    assert(IsStringType(m_Type));

    return std::get<std::string>(m_Data);
}

u64 EngineEvent::GetIntData() const
{
    assert(IsIntType(m_Type));

    return std::get<u64>(m_Data);
}

f32 EngineEvent::GetFloatData() const
{
    assert(IsFloatType(m_Type));

    return std::get<f32>(m_Data);
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
        static_assert(static_cast<u32>(EngineEvent::ArgumentType::Count) == 4);
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
            case EngineEvent::ArgumentType::Void:
            default:
                break;
        }
        static_assert(static_cast<u32>(EngineEvent::ArgumentType::Count) == 4);

        helpText += "\n\t\t\t" + command.m_Description;
    }
    return helpText;
}
