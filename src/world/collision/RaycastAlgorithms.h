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

#pragma once

#include <optional>
#include <vector>

#include <src/world/WorldObjectID.h>

struct TerrainChunk;

struct RaycastResult
{
    f32 m_Distance = -1.f;
    WorldObjectID m_HitObjectID = WORLDOBJECTID_INVALID;
};

class RaycastAlgorithms
{
public:
    static std::optional<RaycastResult> RaycastTerrain(
            const glm::vec3& _localOrigin,
            const glm::vec3& _direction,
            const std::vector<TerrainChunk>& _terrain);
};
