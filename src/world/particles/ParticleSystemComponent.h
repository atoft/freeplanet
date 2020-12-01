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

#pragma once

#include "src/assets/MeshAssets.h"
#include "src/assets/ShaderAssets.h"
#include "src/assets/TextureAssets.h"
#include "src/graphics/ShaderProgram.h"
#include "src/graphics/StaticMesh.h"
#include "src/graphics/Texture.h"
#include <src/world/WorldObjectComponent.h>

struct Particle
{
    glm::vec3 m_RelativePosition = glm::vec3(0.f);

    // An additional particle-space offset.
    glm::vec3 m_OffsetPosition = glm::vec3(0.f);

    glm::mat4 m_Rotation = glm::mat4(1.f);

    glm::vec3 m_Velocity = glm::vec3(0.f);

    Color m_Color = Color(1.f);
    f32 m_TimePassed = 0.f;
};

enum class ParticleAnimation
{
    None,
    PerlinOffset
};

struct ParticleEmitter
{
    std::vector<Particle> m_Particles;

    glm::vec3 m_RelativePosition = glm::vec3(0.f);
    AssetHandle<ShaderProgram> m_Shader;
    AssetHandle<Texture> m_Texture;
    DynamicMeshID m_MeshID;

    ParticleAnimation m_Animation = ParticleAnimation::None;
    f32 m_AnimFrequency = 1.f;
    f32 m_AnimAmplitude = 0.5f;
    f32 m_TimeModulus = 64.f;

    // Emissions per second
    f32 m_EmissionRate = 0.f;

    u32 m_ParticlesPerEmission = 1;
    f32 m_InitialSpeed = 1.f;
    glm::vec3 m_Acceleration = glm::vec3(0.f);

    // Duration a particle exists for after being emitted, seconds.
    f32 m_ParticleLifetime = -1.f;
    f32 m_FadeoutDuration = -1.f;

    f32 m_EmitterLifetime = -1.f;

    bool m_NeedsDepthSort = false;
    
    // Runtime state
    f32 m_TimePassed = 0.f;
    f32 m_TimeSinceEmission = 0.f;
};

struct ParticleSystem
{
    std::vector<ParticleEmitter> m_Emitters;
};

class ParticleSystemComponent : public WorldObjectComponent
{
public:
    ParticleSystemComponent(World* _world, WorldObjectID _ownerID);
    
    ParticleSystem m_ParticleSystem;

    // If true, destroy the owning WorldObject once there are no more particle emitters left.
    bool m_ShouldDestroyOwnerOnFinish = false;
};
