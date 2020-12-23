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
