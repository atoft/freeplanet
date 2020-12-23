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

#include <src/tools/globals.h>

struct Planet;
struct WorldPosition;

class EnvironmentState
{
public:
    EnvironmentState();
    void Update(TimeMS _delta);

    glm::vec3 GetSunDirection() const;
    glm::vec3 GetSunColor() const { return m_SunColor; };
    f32 GetSunIntensity(const Planet* _planet, const WorldPosition& _worldPosition) const;

    glm::vec3 GetAmbientColor() const { return m_AmbientColor; };
    f32 GetAmbientIntensity() const { return m_AmbientIntensity; };

    void SetPlanetaryRotationSpeedScale(f32 _scale) { m_PlanetaryRotationSpeedScale = _scale; };

private:
    glm::mat4x4 m_SunMatrix;

    glm::vec3 m_SunColor = glm::vec3(1.f, 0.988f, 0.878f);
    f32 m_SunIntensity = 0.5f;

    glm::vec3 m_AmbientColor = glm::vec3(0.549, 0.584, 0.909);
    f32 m_AmbientIntensity = 1.f;

    f32 m_PlanetaryRotationSpeedScale = 0.f;
};
