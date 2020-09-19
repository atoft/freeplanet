//
// Created by alastair on 01/08/19.
//


#pragma once

#include <optional>

#include <src/world/WorldObjectID.h>
#include <src/world/WorldPosition.h>
#include <src/world/terrain/TerrainSubstance.h>

// Events that occur in the world. It's possible that there will be listeners
// outside the world (e.g. for playing world sounds or similar) but they should
// only be sent from within the world.
class WorldEvent
{
public:
    enum class Type
    {
        Invalid = 0,
        AddTerrain,
        RemoveTerrain
    };

    WorldObjectID m_Source = WORLDOBJECTID_INVALID;
    std::optional<WorldPosition> m_TargetPosition = std::nullopt;
    std::optional<f32> m_Radius = std::nullopt;
    std::optional<TerrainSubstanceType> m_Substance = std::nullopt;

    Type m_Type = Type::Invalid;
};
