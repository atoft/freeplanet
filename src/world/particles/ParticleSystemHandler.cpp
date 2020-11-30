/* 
 * Copyright 2020 Alastair Toft
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ParticleSystemHandler.h"
#include <glm/fwd.hpp>
#include <glm/gtc/noise.hpp>

#include <src/tools/globals.h>
#include <src/world/particles/ParticleSystemComponent.h>
#include <src/world/World.h>

ParticleSystemHandler::ParticleSystemHandler(World *_world)
    : m_World(_world) {}

void ParticleSystemHandler::Update(TimeMS _delta)
{
    for (WorldZone& zone : m_World->GetActiveZones())
    {
        for (ParticleSystemComponent& particleComponent : zone.GetComponents<ParticleSystemComponent>())
        {
            for (ParticleEmitter& emitter : particleComponent.m_ParticleSystem.m_Emitters)
            {
                UpdateEmitter(emitter, _delta);
            }
        }
    }
}

void ParticleSystemHandler::UpdateEmitter(ParticleEmitter& _emitter, TimeMS _delta)
{
    _emitter.m_TimePassed = glm::mod(_emitter.m_TimePassed + (_delta / 1000.f), _emitter.m_TimeModulus);
    
    switch (_emitter.m_Animation)
    {
    case ParticleAnimation::None:
        break;
    case ParticleAnimation::PerlinOffset:
    {
        for (Particle& particle : _emitter.m_Particles)
        {
            const glm::vec3 noiseSampleDirection = glm::vec3(1.f, 0.f, 0.f);

            // This will look weird once we start rotating spawned trees.
            const glm::vec3 noiseOffsetDirection = glm::vec3(1.f, 0.f, 0.f);

            constexpr f32 noiseSampleScale = 0.25f;

            // TODO Not quite right, causing discontinuity.
            particle.m_OffsetPosition = glm::perlin(
                particle.m_RelativePosition * noiseSampleScale + _emitter.m_TimePassed * _emitter.m_AnimFrequency * noiseSampleDirection,
                glm::vec3(_emitter.m_TimeModulus * _emitter.m_AnimFrequency)) * _emitter.m_AnimAmplitude * noiseOffsetDirection;
        }

        break;
    }
    }
}
