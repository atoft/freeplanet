//
// Created by alastair on 08/09/19.
//

#include "TerrainEdits.h"

#include <src/world/planet/TerrainGeneration.h>

f32 TerrainEdits::GetDensity(glm::vec3 _localPosition) const
{
    f32 density = 0.f;

    for (const TerrainElementVariant& variant : m_AdditiveElements)
    {
        density += TerrainGeneration::ClampDensity(glm::max(std::visit([=](auto&& value){ return value.GetDensity(_localPosition); }, variant), 0.f));
    }

    for (const TerrainElementVariant& variant : m_SubtractiveElements)
    {
        density -= TerrainGeneration::ClampDensity(glm::max(std::visit([=](auto&& value){ return value.GetDensity(_localPosition); }, variant), 0.f));
    }

    return TerrainGeneration::ClampDensity(density);
}

void TerrainEdits::GetSubstance(glm::vec3 _localPosition, TerrainSubstance& _inOutSubstance) const
{
    float highestDensity = -1.f;
    TerrainSubstanceType highestDensitySubstance;

    for (const TerrainElementVariant& variant : m_AdditiveElements)
    {
        const float elementDensity = std::visit([=](auto&& value){ return value.GetDensity(_localPosition); }, variant);

        if (elementDensity > highestDensity)
        {
            highestDensity = elementDensity;
            highestDensitySubstance = std::visit([=](auto&& value){ return value.GetSubstance(); }, variant);
        }
    }

    constexpr float ELEMENT_CONSIDERED_FOR_SUBSTANCE_THRESHOLD = -0.5f;

    if (highestDensity >= ELEMENT_CONSIDERED_FOR_SUBSTANCE_THRESHOLD)
    {
        _inOutSubstance = TerrainSubstance::FromType(highestDensitySubstance);
    }
}

bool TerrainEdits::IsEmpty() const
{
    return m_AdditiveElements.empty() && m_SubtractiveElements.empty();
}
