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

#include <glm/glm.hpp>

class WorldZone;

struct WorldPosition
{
    WorldPosition(const glm::ivec3& _zoneCoordinates, const glm::vec3& _localPosition);
    WorldPosition operator+(glm::vec3 _other) const;
    WorldPosition operator-(glm::vec3 _other) const;

    glm::vec3 GetPositionRelativeTo(glm::ivec3 _zoneCoordinates) const;
    glm::vec3 GetPositionRelativeTo(const WorldZone& _zone) const;

    bool IsInsideZone() const;
    WorldPosition GetInsideZone() const;
    
    glm::ivec3 m_ZoneCoordinates;
    glm::vec3 m_LocalPosition;
};
