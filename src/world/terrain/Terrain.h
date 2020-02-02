//
// Created by alastair on 08/09/19.
//

#pragma once

#include <variant>
#include <vector>

#include <src/world/terrain/elements/BoxTerrainElement.h>
#include <src/world/terrain/elements/PerlinTerrainElement.h>
#include <src/world/terrain/elements/PlaneTerrainElement.h>
#include <src/world/terrain/elements/SphereTerrainElement.h>

using TerrainElementVariant = std::variant<PerlinTerrainElement, PlaneTerrainElement, SphereTerrainElement, BoxTerrainElement>;

struct Terrain
{
    f32 GetDensity(glm::vec3 _localPosition) const;
    bool IsEmpty() const;

    std::vector<TerrainElementVariant> m_AdditiveElements;
    std::vector<TerrainElementVariant> m_SubtractiveElements;
    glm::vec3 m_GlobalPositionOffset;
};