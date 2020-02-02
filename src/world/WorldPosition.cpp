//
// Created by alastair on 01/08/19.
//

#include "WorldPosition.h"

#include <src/tools/globals.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/WorldZone.h>

WorldPosition::WorldPosition(const glm::ivec3& _zoneCoordinates, const glm::vec3& _localPosition)
    : m_ZoneCoordinates(_zoneCoordinates)
    , m_LocalPosition(_localPosition)
{}

WorldPosition WorldPosition::operator+(glm::vec3 _other)
{
    // HACK Need to handle the case where the offset pushes the position into a different zone!
    return {this->m_ZoneCoordinates, this->m_LocalPosition + _other};
}

glm::vec3 WorldPosition::GetPositionRelativeTo(glm::ivec3 _zoneCoordinates) const
{
    return glm::vec3(m_ZoneCoordinates - _zoneCoordinates) * TerrainConstants::WORLD_ZONE_SIZE + m_LocalPosition;
}

glm::vec3 WorldPosition::GetPositionRelativeTo(const WorldZone& _zone) const
{
    return GetPositionRelativeTo(_zone.GetCoordinates());
}


