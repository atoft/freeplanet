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

#include "InventoryHandler.h"
#include "src/assets/MeshAssets.h"
#include "src/assets/ShaderAssets.h"
#include "src/graphics/Scene.h"
#include "src/world/LightComponent.h"
#include "src/world/SpawningHandler.h"

#include <src/assets/TextureAssets.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/collision/CollisionHandler.h>
#include <src/world/planet/PlanetGeneration.h>
#include <src/world/World.h>

InventoryHandler::InventoryHandler(World *_world)
    : m_World(_world)
{}

void InventoryHandler::RegisterLocalPlayer(u32 _playerIndex)
{
    Inventory& defaultInventory = m_Inventories.emplace_back();

    constexpr f32 spawnedLifetime = 20.f;
    
    PropRecipe campfireRecipe;
    campfireRecipe.m_MeshID = MeshAsset_Campfire;
    campfireRecipe.m_ShaderID = ShaderAsset_Lit_Textured;
    campfireRecipe.m_TextureID = TextureAsset_Campfire;
    campfireRecipe.m_ParticleSystem = ParticleSystem();
    campfireRecipe.m_Light = LightEmitter();
    campfireRecipe.m_Light->m_Brightness = 1.f;
    campfireRecipe.m_Light->m_Color = Color(1.f);
    campfireRecipe.m_Light->m_LocalOffset = glm::vec3(0.f, 1.f, 0.f);
    
    ParticleEmitter& fireEmitter = campfireRecipe.m_ParticleSystem->m_Emitters.emplace_back();
    fireEmitter.m_ParticlesPerEmission = 6;
    fireEmitter.m_EmissionRate = 4.f;
    fireEmitter.m_Acceleration = glm::vec3(0.f, 2.f, 0.f);
    fireEmitter.m_ParticleLifetime = 1.f;
    fireEmitter.m_FadeoutDuration = 0.6f;
    fireEmitter.m_InitialSpeed = 0.5f;
    // HACK: Too early to do this.    fireEmitter.m_MeshID = m_World->GetSpawningHandler()->m_ParticleSystemMesh.GetID();
    fireEmitter.m_Shader = ShaderAsset_Lit_Inst_AlphaBlend_NormalUp;
    fireEmitter.m_Texture = TextureAsset_Billboard_Flame;
    fireEmitter.m_UseParticleColor = true;
    fireEmitter.m_BaseColor = Color(0.976f, 0.694f, 0.462f, 1.f);
    fireEmitter.m_NeedsDepthSort = true;
    fireEmitter.m_Blending = Renderable::AlphaBlending::Additive;
    fireEmitter.m_EmitterLifetime = spawnedLifetime;
    
    ParticleEmitter& smokeEmitter = campfireRecipe.m_ParticleSystem->m_Emitters.emplace_back();
    smokeEmitter.m_ParticlesPerEmission = 1;
    smokeEmitter.m_EmissionRate = 2.f;
    smokeEmitter.m_Acceleration = glm::vec3(0.5f, 2.f, 0.f);
    smokeEmitter.m_ParticleLifetime = 4.f;
    smokeEmitter.m_FadeoutDuration = 2.f;
    smokeEmitter.m_InitialSpeed = 0.f;
    // HACK: Too early to do this.    smokeEmitter.m_MeshID = m_World->GetSpawningHandler()->m_ParticleSystemMesh.GetID();
    smokeEmitter.m_Shader = ShaderAsset_Lit_Inst_AlphaBlend_NormalUp;
    smokeEmitter.m_Texture = TextureAsset_Billboard_Smoke;
    smokeEmitter.m_UseParticleColor = true;
    smokeEmitter.m_BaseColor = Color(0.8f, 0.8f, 0.8f, 1.f);
    smokeEmitter.m_NeedsDepthSort = true;
    smokeEmitter.m_Blending = Renderable::AlphaBlending::Blend;
    smokeEmitter.m_EmitterLifetime = spawnedLifetime;

    
    defaultInventory.m_PlayerID = _playerIndex;
    defaultInventory.m_Slots =
        {
            {"Topsoil", TerrainSubstanceType::Topsoil, 100, AssetHandle<UITexture>(TextureAsset_Icon_Topsoil512), std::nullopt},
            {"Dirt", TerrainSubstanceType::Dirt, 100, AssetHandle<UITexture>(TextureAsset_Icon_Dirt512), std::nullopt},
            {"Rock", TerrainSubstanceType::Rock, 100, AssetHandle<UITexture>(TextureAsset_Icon_Stone512), std::nullopt},
            {"Sand", TerrainSubstanceType::Sand, 100, AssetHandle<UITexture>(TextureAsset_Icon_Sand512), std::nullopt},
            {"Campfire", std::nullopt, 100, AssetHandle<UITexture>(TextureAsset_Icon_Fire512), campfireRecipe},
        };

}

void InventoryHandler::OnButtonInput(u32 _playerIdx, WorldObject* _controlledWorldObject, InputType _inputType)
{
    if (_inputType == InputType::Interact || _inputType == InputType::InteractAlternate)
    {
        const FreelookCameraComponent* camera = ComponentAccess::GetComponent<FreelookCameraComponent>(*_controlledWorldObject);

        const glm::vec3 lookDirection = camera->GetLookDirection();
        std::optional<f32> rayIntersect = m_World->GetCollisionHandler()->DoRaycast(_controlledWorldObject->GetWorldPosition(), lookDirection);

        if (rayIntersect == std::nullopt)
        {
            return;
        }

        InventorySlot& slot = m_Inventories[_playerIdx].m_Slots[m_Inventories[_playerIdx].m_SelectedIndex];

        if (slot.m_Substance.has_value())
        {
            WorldEvent terrainEvent;
     
            if (_inputType == InputType::Interact)
            {
                terrainEvent.m_Type = WorldEvent::Type::AddTerrain;
                terrainEvent.m_Substance = slot.m_Substance;
                slot.m_Count--;
            }
            else if (_inputType == InputType::InteractAlternate)
            {
                terrainEvent.m_Type = WorldEvent::Type::RemoveTerrain;
            }
     
            constexpr f32 TERRAIN_INTERACTION_RADIUS = 2.f;
     
            terrainEvent.m_Radius = TERRAIN_INTERACTION_RADIUS;
            terrainEvent.m_Source = _controlledWorldObject->GetWorldObjectID() ;
            terrainEvent.m_TargetPosition = _controlledWorldObject->GetWorldPosition() + lookDirection * (*rayIntersect);
     
            m_World->AddWorldEvent(terrainEvent);
        }
        else if (slot.m_Prop.has_value())
        {
            WorldPosition targetWorldPosition = _controlledWorldObject->GetWorldPosition() + lookDirection * (*rayIntersect);
            targetWorldPosition.GetInsideZone();
            
            WorldZone* targetZone = m_World->FindZoneAtCoordinates(targetWorldPosition.m_ZoneCoordinates);
            assert(targetZone != nullptr);

            WorldObject& spawnedObject = m_World->ConstructWorldObject(*targetZone, "Tree");
            spawnedObject.SetInitialPosition(targetWorldPosition.m_LocalPosition);

            const glm::mat3x3 rotationMatrix = MathsHelpers::GenerateRotationMatrix3x3FromUp(
                PlanetGeneration::GetUpDirection(*m_World->GetPlanet(), spawnedObject.GetWorldPosition()));

            MathsHelpers::SetRotationPart(spawnedObject.GetZoneTransform(), rotationMatrix);

            RenderComponent& renderComponent = targetZone->AddComponent<RenderComponent>
                    (spawnedObject,
                     AssetHandle<StaticMesh>(slot.m_Prop->m_MeshID),
                     AssetHandle<ShaderProgram>(slot.m_Prop->m_ShaderID),
                     AssetHandle<Texture>(slot.m_Prop->m_TextureID),
                     MeshType::Normal);

            renderComponent.m_CanInstance = false;
            
            if (slot.m_Prop->m_ParticleSystem.has_value())
            {
                ParticleSystemComponent& particleSystem = targetZone->AddComponent<ParticleSystemComponent>(spawnedObject);
                particleSystem.m_ParticleSystem = *slot.m_Prop->m_ParticleSystem;
                particleSystem.m_ShouldDestroyOwnerOnFinish = true;
                
                for (ParticleEmitter& emitter : particleSystem.m_ParticleSystem.m_Emitters)
                {
                    emitter.m_MeshID = m_World->GetSpawningHandler()->m_ParticleSystemMesh.GetID();
                }
            }

            if (slot.m_Prop->m_Light.has_value())
            {
                LightComponent& light = targetZone->AddComponent<LightComponent>(spawnedObject);
                light.m_Emitter = *slot.m_Prop->m_Light;
            }
        }
        return;
    }

    switch (_inputType)
    {
    case InputType::InventorySlot1:
    {
        ChangeSlot(_playerIdx, 0);
        break;
    }
    case InputType::InventorySlot2:
    {
        ChangeSlot(_playerIdx, 1);
        break;
    }
    case InputType::InventorySlot3:
    {
        ChangeSlot(_playerIdx, 2);
        break;
    }
    case InputType::InventorySlot4:
    {
        ChangeSlot(_playerIdx, 3);
        break;
    }
    case InputType::InventorySlot5:
    {
        ChangeSlot(_playerIdx, 4);
        break;
    }

    default:
        break;
    }
}

void InventoryHandler::ChangeSlot(u32 _playerIndex, u32 _slotIndex)
{
    for (Inventory& inventory : m_Inventories)
    {
        if (inventory.m_PlayerID == _playerIndex)
        {
            if (m_Inventories[_playerIndex].m_Slots.size() > _slotIndex)
            {
                m_Inventories[_playerIndex].m_SelectedIndex = _slotIndex;
            }
            break;
        }
    }
}

const Inventory& InventoryHandler::GetInventory(u32 _playerIndex) const
{
    return m_Inventories[_playerIndex];
}
