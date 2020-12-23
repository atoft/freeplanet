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
