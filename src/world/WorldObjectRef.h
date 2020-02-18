//
// Created by alastair on 17/02/2020.
//

#pragma once

#include <src/tools/globals.h>

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
