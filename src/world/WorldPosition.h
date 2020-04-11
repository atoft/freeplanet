//
// Created by alastair on 01/08/19.
//

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

    glm::ivec3 m_ZoneCoordinates;
    glm::vec3 m_LocalPosition;
};
