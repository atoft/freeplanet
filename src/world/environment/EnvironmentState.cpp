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

#include "EnvironmentState.h"

#include <glm/gtx/matrix_decompose.hpp>

#include <src/tools/MathsHelpers.h>
#include <src/world/WorldPosition.h>
#include <src/world/planet/Planet.h>
#include <src/world/planet/PlanetGeneration.h>

EnvironmentState::EnvironmentState()
{
    m_SunMatrix = glm::rotate(glm::mat4(1.f), glm::quarter_pi<f32>(), glm::vec3(1,0,0));
}

void EnvironmentState::Update(TimeMS _delta)
{
    constexpr f32 SUN_MOVE_RATE = 0.00025f;

    m_SunMatrix = glm::rotate(m_SunMatrix, SUN_MOVE_RATE * _delta * m_PlanetaryRotationSpeedScale, glm::vec3(1,0,0));

}

glm::vec3 EnvironmentState::GetSunDirection() const
{
    return m_SunMatrix * glm::vec4(0,1,0,1);
}

f32 EnvironmentState::GetSunIntensity(const Planet* _planet, const WorldPosition& _worldPosition) const
{
    const glm::vec3 upDirection = _planet != nullptr ? PlanetGeneration::GetUpDirection(*_planet, _worldPosition) : glm::vec3(0,1,0);

    return m_SunIntensity * glm::smoothstep(-0.2f, 0.2f, glm::dot(GetSunDirection(), upDirection));
}
