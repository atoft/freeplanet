//
// Created by alastair on 01/08/19.
//

#include "WorldPosition.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/vector_relational.hpp>
#include <src/tools/globals.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/WorldZone.h>

WorldPosition::WorldPosition(const glm::ivec3& _zoneCoordinates, const glm::vec3& _localPosition)
    : m_ZoneCoordinates(_zoneCoordinates)
    , m_LocalPosition(_localPosition)
{}

WorldPosition WorldPosition::operator+(glm::vec3 _other) const
{
    return WorldPosition(this->m_ZoneCoordinates, this->m_LocalPosition + _other).GetInsideZone();
}

WorldPosition WorldPosition::operator-(glm::vec3 _other) const
{
    return operator+(-_other);
}

glm::vec3 WorldPosition::GetPositionRelativeTo(glm::ivec3 _zoneCoordinates) const
{
    return glm::vec3(m_ZoneCoordinates - _zoneCoordinates) * TerrainConstants::WORLD_ZONE_SIZE + m_LocalPosition;
}

glm::vec3 WorldPosition::GetPositionRelativeTo(const WorldZone& _zone) const
{
    return GetPositionRelativeTo(_zone.GetCoordinates());
}

bool WorldPosition::IsInsideZone() const
{
    return glm::all(glm::greaterThanEqual(m_LocalPosition, glm::vec3(-TerrainConstants::WORLD_ZONE_SIZE / 2.f)))
        && glm::all(glm::lessThan(m_LocalPosition, glm::vec3(TerrainConstants::WORLD_ZONE_SIZE / 2.f)));
}

void ClampAndIncrementCoordinate(s32& _zoneCoordinate, f32& _localPosition)
{
    if (_localPosition >= TerrainConstants::WORLD_ZONE_SIZE / 2.f)
    {
        _localPosition -= TerrainConstants::WORLD_ZONE_SIZE;
        ++_zoneCoordinate;
    }
    else if (_localPosition < -TerrainConstants::WORLD_ZONE_SIZE / 2.f)
    {   
        _localPosition += TerrainConstants::WORLD_ZONE_SIZE;
        --_zoneCoordinate;
    }
}

WorldPosition WorldPosition::GetInsideZone() const
{
    WorldPosition result = *this;

    while (!result.IsInsideZone())
    {
        ClampAndIncrementCoordinate(result.m_ZoneCoordinates.x, result.m_LocalPosition.x);
        ClampAndIncrementCoordinate(result.m_ZoneCoordinates.y, result.m_LocalPosition.y);
        ClampAndIncrementCoordinate(result.m_ZoneCoordinates.z, result.m_LocalPosition.z);
    }
    return result;
}

