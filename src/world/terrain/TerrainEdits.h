//
// Created by alastair on 08/09/19.
//

#pragma once

#include <variant>
#include <vector>

#include <src/world/terrain/elements/BoxTerrainElement.h>
#include <src/world/terrain/elements/SphereTerrainElement.h>

using TerrainElementVariant = std::variant<SphereTerrainElement, BoxTerrainElement>;

struct TerrainEdits
{
    f32 GetDensity(glm::vec3 _localPosition) const;
    bool IsEmpty() const;

    std::vector<TerrainElementVariant> m_AdditiveElements;
    std::vector<TerrainElementVariant> m_SubtractiveElements;
};