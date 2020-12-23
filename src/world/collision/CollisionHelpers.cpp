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
#define GLM_ENABLE_EXPERIMENTAL

#include "CollisionHelpers.h"

#include <glm/vec3.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <optional>

#include <src/tools/MathsHelpers.h>
#include <src/world/WorldObject.h>
#include <src/world/GeometryTypes.h>

bool CollisionHelpers::IsIntersecting(glm::vec3 _position1, AABB _aabb1, glm::vec3 _position2,
                                      AABB _aabb2)
{
    _position1 += _aabb1.m_PositionOffset;
    _position2 += _aabb2.m_PositionOffset;

    glm::vec3 halfDimensions1 = _aabb1.m_Dimensions;
    glm::vec3 halfDimensions2 = _aabb2.m_Dimensions;

    int axis = 0;
    for(; axis < 3; axis++)
    {
        if(!CollisionInAxis(axis, _position1, halfDimensions1, _position2, halfDimensions2))
            return false;
    }
    return true;
}

bool CollisionHelpers::CollisionInAxis(int _index, glm::vec3 _position1, glm::vec3 _halfDimensions1, glm::vec3 _position2,
                                       glm::vec3 _halfDimensions2)
{
    return ( glm::abs(_position1[_index] - _position2[_index]) < (_halfDimensions1[_index] + _halfDimensions2[_index])  );
}

glm::vec3 CollisionHelpers::Rotate(glm::vec3 position, glm::vec3 eulerAngles)
{
    return glm::rotateX(glm::rotateY(glm::rotateZ(position, eulerAngles.z), eulerAngles.y), eulerAngles.x);
}

CollisionHelpers::OBBProperties CollisionHelpers::GetOBBProperties(const glm::mat4& _transform, const AABB& _bounds, const glm::vec3& _positionOffset)
{
    CollisionHelpers::OBBProperties properties;

    properties.m_Origin = MathsHelpers::GetPosition(_transform) + _bounds.m_PositionOffset - _positionOffset;
    properties.m_X = glm::normalize(MathsHelpers::GetRightVector(_transform));
    properties.m_Y = glm::normalize(MathsHelpers::GetUpVector(_transform));
    properties.m_Z = glm::normalize(MathsHelpers::GetForwardVector(_transform));

    const glm::vec3 obbExtents = _bounds.m_Dimensions;

    properties.m_Coordinates[0] = properties.m_Origin + properties.m_X * obbExtents.x + properties.m_Y * obbExtents.y + properties.m_Z * obbExtents.z;
    properties.m_Coordinates[1] = properties.m_Origin - properties.m_X * obbExtents.x + properties.m_Y * obbExtents.y + properties.m_Z * obbExtents.z;

    properties.m_Coordinates[2] = properties.m_Origin + properties.m_X * obbExtents.x - properties.m_Y * obbExtents.y + properties.m_Z * obbExtents.z;
    properties.m_Coordinates[3] = properties.m_Origin - properties.m_X * obbExtents.x - properties.m_Y * obbExtents.y + properties.m_Z * obbExtents.z;

    properties.m_Coordinates[4] = properties.m_Origin + properties.m_X * obbExtents.x + properties.m_Y * obbExtents.y - properties.m_Z * obbExtents.z;
    properties.m_Coordinates[5] = properties.m_Origin - properties.m_X * obbExtents.x + properties.m_Y * obbExtents.y - properties.m_Z * obbExtents.z;

    properties.m_Coordinates[6] = properties.m_Origin + properties.m_X * obbExtents.x - properties.m_Y * obbExtents.y - properties.m_Z * obbExtents.z;
    properties.m_Coordinates[7] = properties.m_Origin - properties.m_X * obbExtents.x - properties.m_Y * obbExtents.y - properties.m_Z * obbExtents.z;

    return properties;
}

AABB CollisionHelpers::GetAABBForOBB(const glm::mat4& _rotation, const AABB& _bounds)
{
    glm::vec3 basisX = glm::abs(glm::vec4(1,0,0,1) * glm::inverse(_rotation)) * _bounds.m_Dimensions.x;
    glm::vec3 basisY = glm::abs(glm::vec4(0,1,0,1) * glm::inverse(_rotation)) * _bounds.m_Dimensions.y;
    glm::vec3 basisZ = glm::abs(glm::vec4(0,0,1,1) * glm::inverse(_rotation)) * _bounds.m_Dimensions.z;

    AABB result;
    result.m_PositionOffset = _bounds.m_PositionOffset;
    result.m_Dimensions = basisX + basisY + basisZ;

    return result;
}
