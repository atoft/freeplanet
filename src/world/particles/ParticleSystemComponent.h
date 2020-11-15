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
    
    // TODO lifetime, velocity, etc.
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

    // TODO properties for emission rate, initial velocity, etc.

    ParticleAnimation m_Animation = ParticleAnimation::None;
    f32 m_AnimFrequency = 1.f;
    f32 m_AnimAmplitude = 0.5f;
    f32 m_TimePassed = 0.f;
    f32 m_TimeModulus = 64.f;
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
};
