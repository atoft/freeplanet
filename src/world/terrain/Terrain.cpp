//
// Created by alastair on 08/09/19.
//

#include "Terrain.h"

f32 Terrain::GetDensity(glm::vec3 _localPosition) const
{
    f32 density = 0.f;

    for (const TerrainElementVariant& variant : m_AdditiveElements)
    {
        density += std::visit([=](auto&& value){ return value.GetDensity(_localPosition, m_GlobalPositionOffset); }, variant);
    }

    for (const TerrainElementVariant& variant : m_SubtractiveElements)
    {
        density -= std::visit([=](auto&& value){ return value.GetDensity(_localPosition, m_GlobalPositionOffset); }, variant);
    }

    return density;
}

bool Terrain::IsEmpty() const
{
    return m_AdditiveElements.empty() && m_SubtractiveElements.empty();
}
