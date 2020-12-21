//
// Created by alastair on 11/11/19.
//

#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <utility>

#include <src/assets/MeshAssets.h>
#include <src/assets/ShaderAssets.h>
#include <src/assets/TextureAssets.h>
#include <src/world/LightComponent.h>
#include <src/world/particles/ParticleSystemComponent.h>

// Specifies some parameters to use when constructing a prop, for testing.
// Will probably do something similar in the procedural system for
// generating WorldObjects in general.
struct PropRecipe
{
    std::string m_Name;

    glm::vec3 m_PitchYawRoll = glm::vec3();
    glm::vec3 m_Scale = glm::vec3(1.f);

    MeshAssetID m_MeshID = MeshAsset_Invalid;
    ShaderAssetID m_ShaderID = ShaderAsset_Invalid;
    TextureAssetID m_TextureID = TextureAsset_Invalid;
    std::optional<ParticleSystem> m_ParticleSystem;
    std::optional<LightEmitter> m_Light;
};
