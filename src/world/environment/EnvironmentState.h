//
// Created by alastair on 23/06/19.
//

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
