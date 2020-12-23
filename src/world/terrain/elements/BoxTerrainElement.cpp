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

#include "BoxTerrainElement.h"

BoxTerrainElement::BoxTerrainElement(const glm::vec3& _position, const glm::vec3& _extents, f32 _transitionSize)
    : m_Position(_position),
      m_Extents(_extents) {}

f32 BoxTerrainElement::GetDensity(glm::vec3 position) const
{
    const f32 dX = position.x > m_Position.x - m_Extents.x && position.x < m_Position.x + m_Extents.x ? 10.f : 0.f;
    const f32 dY = position.y > m_Position.y - m_Extents.y && position.y < m_Position.y + m_Extents.y ? 10.f : 0.f;
    const f32 dZ = position.z > m_Position.z - m_Extents.z && position.z < m_Position.z + m_Extents.z ? 10.f : 0.f;

    return glm::min(dX, glm::min(dY, dZ));
}


