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
#include "src/world/WorldObjectID.h"
#include <glm/fwd.hpp>
#include <glm/gtc/noise.hpp>

#include <random>
#include <src/tools/globals.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/particles/ParticleSystemComponent.h>
#include <src/world/World.h>

ParticleSystemHandler::ParticleSystemHandler(World *_world)
    : m_World(_world) {}

void ParticleSystemHandler::Update(TimeMS _delta)
{
    std::vector<WorldObjectID> particleObjectsToDestroy;
    
    for (WorldZone& zone : m_World->GetActiveZones())
    {
        for (ParticleSystemComponent& particleComponent : zone.GetComponents<ParticleSystemComponent>())
        {
            STL::RemoveIf(particleComponent.m_ParticleSystem.m_Emitters, [](const ParticleEmitter& _emitter)
                                                                         {
                                                                             return _emitter.m_EmitterLifetime > 0.f
                                                                                 && _emitter.m_TimePassed > _emitter.m_EmitterLifetime
                                                                                 && _emitter.m_Particles.empty();
                                                                         });
            
            for (ParticleEmitter& emitter : particleComponent.m_ParticleSystem.m_Emitters)
            {
                UpdateEmitter(emitter, _delta);
            }

            if (particleComponent.m_ShouldDestroyOwnerOnFinish && particleComponent.m_ParticleSystem.m_Emitters.empty())
            {
                particleObjectsToDestroy.push_back(particleComponent.GetOwner());
            }
        }
    }

    for (WorldObjectID id : particleObjectsToDestroy)
    {
        m_World->DestroyWorldObject(id);
    }
}

void ParticleSystemHandler::UpdateEmitter(ParticleEmitter& _emitter, TimeMS _delta)
{
    const bool isFirstUpdate = _emitter.m_TimePassed == 0.f;
    _emitter.m_TimePassed = glm::mod(_emitter.m_TimePassed + (_delta / 1000.f), _emitter.m_TimeModulus);

    // Remove particles
    if (_emitter.m_ParticleLifetime > 0.f)
    {
        STL::RemoveIf(_emitter.m_Particles, [&_emitter](const Particle &_particle)
                                          {
                                              return _particle.m_TimePassed > _emitter.m_ParticleLifetime;
                                          });
    }
    
    // Emit particles
    if (_emitter.m_EmissionRate > 0.f && (_emitter.m_EmitterLifetime < 0.f || _emitter.m_TimePassed < _emitter.m_EmitterLifetime))
    {
        if (isFirstUpdate || _emitter.m_TimeSinceEmission > 1.f / _emitter.m_EmissionRate)
        {
            std::uniform_real_distribution<f32> dist;
            const f32 offset = dist(m_RandomGenerator);
            
            for (u32 emittedIdx = 0; emittedIdx < _emitter.m_ParticlesPerEmission; ++emittedIdx)
            {
                Particle& particle = _emitter.m_Particles.emplace_back();
                
                const glm::vec3 emissionDirection = MathsHelpers::GenerateNormalFromPitchYaw(
                    0.f,
                    offset * glm::pi<f32>() * 0.5f + static_cast<f32>(emittedIdx) / static_cast<f32>(_emitter.m_ParticlesPerEmission) * 2.f * glm::pi<f32>());

                particle.m_Velocity = emissionDirection * _emitter.m_InitialSpeed;
                particle.m_Rotation = MathsHelpers::GenerateRotationMatrixFromUp(emissionDirection);
                particle.m_TimePassed = 0.f;
                particle.m_Color = _emitter.m_BaseColor;
            }

            _emitter.m_TimeSinceEmission = 0.f;
        }
        else
        {
            _emitter.m_TimeSinceEmission += _delta / 1000.f;
        }
    }

    // Tick particles
    for (Particle& particle : _emitter.m_Particles)
    {
        particle.m_Velocity += _emitter.m_Acceleration * (_delta / 1000.f);

        particle.m_RelativePosition += particle.m_Velocity * (_delta / 1000.f);

        particle.m_TimePassed += _delta / 1000.f;

        if (_emitter.m_FadeoutDuration > 0.f)
        {
            const f32 fadeProgress = glm::clamp((particle.m_TimePassed - (_emitter.m_ParticleLifetime - _emitter.m_FadeoutDuration)) / _emitter.m_FadeoutDuration, 0.f, 1.f);
            const f32 alpha = 1.f - fadeProgress;

            particle.m_Color.a = alpha;
        }
    }

    // Apply ParticleAnimation
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

            particle.m_OffsetPosition = glm::perlin(
                particle.m_RelativePosition * noiseSampleScale + _emitter.m_TimePassed * _emitter.m_AnimFrequency * noiseSampleDirection,
                glm::vec3(_emitter.m_TimeModulus * _emitter.m_AnimFrequency)) * _emitter.m_AnimAmplitude * noiseOffsetDirection;
        }

        break;
    }
    }
}
