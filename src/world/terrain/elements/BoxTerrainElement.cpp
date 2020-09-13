//
// Created by alastair on 01/10/19.
//

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


