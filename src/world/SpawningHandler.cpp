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

#include "SpawningHandler.h"

#include <random>
#include <glm/gtx/norm.hpp>

#include <src/graphics/MeshImport.h>
#include <src/graphics/RenderComponent.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/World.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/planet/PlanetGeneration.h>
#include <src/engine/AssetBank.h>
#include <src/graphics/DynamicMeshHandle.h>
#include <src/world/flora/FloraGeneration.h>


SpawningHandler::SpawningHandler(World* _world)
    : m_World(_world)
{}

// HACK To work around the limitations of static mesh. We don't have a way to
// say that we want a version of a static mesh that supports instancing.
RawMesh ImportMeshWithInstancing(MeshAssetID _id)
{
    RawMesh result;
    
    std::string modelPath = Globals::FREEPLANET_ASSET_PATH + "models/" + Assets::GetMeshAssetName(_id) + ".obj";
    
    std::optional<MeshImport::ImportedMeshData> mesh = MeshImport::ImportOBJ(modelPath);

    if (!mesh.has_value())
    {
        LogError("Failed to import " + modelPath);
        return result;
    }

    result = MeshImport::ConvertToRawMesh(*mesh);
    result.m_SupportInstancing = true;
    
    return result;
}


void SpawningHandler::Update()
{
    const Planet* planet = m_World->GetPlanet();

    if (planet == nullptr)
    {
        return;
    }

    // TODO: Actually, this happens per biome, and we need to be smart about when it occurs.
    // Will most likely need a DynamicLoader to do this async once different biomes have different plants.
    if (m_PlantInstances.empty())
    {
        // HACK To work around the limitations of static mesh. We don't have a way to
        // say that we want a version of a static mesh that supports instancing.

        const RawMesh particleMesh = ImportMeshWithInstancing(MeshAsset_UnitQuad);

        DynamicMeshHandle particleMeshHandle;
        particleMeshHandle.RequestMeshUpdate(particleMesh);
        m_ParticleSystemMesh = particleMeshHandle;
        
        for (u32 seed = 0; seed < 4; ++seed)
        {
            const PlantInstance plantInstance = FloraGeneration::GeneratePlant(FloraGenerationParams(), seed);

            const RawMesh plantRawMesh = FloraGeneration::ConvertToRawMesh(plantInstance, FloraGenerationParams()); 
            
            DynamicMeshHandle plantMeshHandle;
            plantMeshHandle.RequestMeshUpdate(plantRawMesh);

            m_PlantInstances.push_back(plantInstance);
            m_SpawnedPlantMeshes.push_back(plantMeshHandle);
            
            ParticleSystem foliageParticles = FloraGeneration::GenerateFoliage(plantInstance, FloraGenerationParams());

            m_SpawnedParticleSystems.push_back(foliageParticles);
        }
    }

    for (const DynamicMeshHandle& handle : m_SpawnedPlantMeshes)
    {
        if (!handle.IsUpToDate())
        {
            // Don't spawn until the meshes are ready.
            return;
        }
    }

    for (WorldZone& zone : m_World->GetActiveZones())
    {
        if (!zone.IsProceduralSpawningDone() && zone.GetTerrainComponent().m_HasFinishedFirstUpdate)
        {
            // Use the terrain's RawMesh to allow us to query vertices directly.
            // This approach is not ideal since it prevents us from spawning objects in the same place in the vista as
            // in the actual world.
            const RawMesh& rawMesh = zone.GetTerrainComponent().m_DynamicMesh.GetRawMesh();

            constexpr f32 fractionOfVertsToSpawnTreesOn = 0.015f;
            constexpr f32 fractionOfVertsToSpawnGrassOn = 0.125f;

            const Planet::Biome& biome = TerrainGeneration::GetBiome(*planet, WorldPosition(zone.GetCoordinates(), glm::vec3(0.f)));

            const u32 targetTreeQuantity = biome.m_TreeCoverage * fractionOfVertsToSpawnTreesOn * rawMesh.m_Vertices.size();
            const u32 targetGrassQuantity = biome.m_TreeCoverage * fractionOfVertsToSpawnGrassOn * rawMesh.m_Vertices.size();

            // TODO probably need to mutate the seed based on the zone to avoid an identical shuffle for every zone.
            std::mt19937 gen(planet->m_TerrainSeed);

            std::vector<glm::vec3> verts = rawMesh.m_Vertices;
            std::shuffle(verts.begin(), verts.end(), gen);

            u32 spawnedCount = 0;
            auto vertIt = verts.begin();
            while (spawnedCount < (targetTreeQuantity + targetGrassQuantity) && vertIt < verts.end())
            {
                const glm::vec3 spawnPosition = TerrainHelpers::ToLocalSpace(*vertIt, zone.GetTerrainComponent().m_Properties);

                ++vertIt;

                if (glm::any(glm::greaterThanEqual(glm::abs(spawnPosition), glm::vec3(TerrainConstants::WORLD_ZONE_SIZE / 2.f))))
                {
                    continue;
                }

                // TODO Verify if the position is safe to spawn at - surface normal, vertical space, horizontal space.

                if (spawnedCount < targetTreeQuantity)
                {
                    WorldObject& worldObject = m_World->ConstructWorldObject(zone, "Tree");
                    worldObject.SetInitialPosition(spawnPosition);

                    const glm::mat3x3 rotationMatrix = MathsHelpers::GenerateRotationMatrix3x3FromUp(
                            PlanetGeneration::GetUpDirection(*m_World->GetPlanet(), worldObject.GetWorldPosition()));

                    MathsHelpers::SetRotationPart(worldObject.GetZoneTransform(), rotationMatrix);

                    const u32 meshIdx = spawnedCount % m_SpawnedPlantMeshes.size();
                    
                    RenderComponent& renderComponent = zone.AddComponent<RenderComponent>
                            (worldObject,
                             AssetHandle<StaticMesh>(ASSETID_INVALID),
                             FloraGenerationParams().m_BranchShader,
                             FloraGenerationParams().m_BranchTexture,
                             MeshType::Normal,
                             m_SpawnedPlantMeshes[meshIdx].GetID());

                    renderComponent.m_CanInstance = true;
                    
                    ParticleSystemComponent& particleSystem = zone.AddComponent<ParticleSystemComponent>(worldObject);
                    particleSystem.m_ParticleSystem = m_SpawnedParticleSystems[meshIdx];
                    particleSystem.m_ParticleSystem.m_Emitters[0].m_MeshID = m_ParticleSystemMesh.GetID();

                    ColliderComponent& collider = zone.AddComponent<ColliderComponent>(worldObject,
                                                                                       CollisionPrimitiveType::OBB,
                                                                                       MovementType::Movable);
                    collider.m_Bounds = AABB(glm::vec3(.8f, 4.f, .8f), glm::vec3(0.f, 2.f, 0.f));
                    collider.m_MovementType = MovementType::Fixed;
                }
                else
                {
                    // Probably(?) don't want to go as far as having a WorldObject for every blade of grass. Probably
                    // do it as a particle system per zone.
                    WorldObject &worldObject = m_World->ConstructWorldObject(zone, "Grass");
                    worldObject.SetInitialPosition(spawnPosition + glm::vec3(0.f, 0.5f, 0.f));

                    const glm::mat3x3 rotationMatrix = MathsHelpers::GenerateRotationMatrix3x3FromUp(
                            PlanetGeneration::GetUpDirection(*m_World->GetPlanet(), worldObject.GetWorldPosition()));

                    MathsHelpers::SetRotationPart(worldObject.GetZoneTransform(), rotationMatrix);

                    RenderComponent& renderComponent = zone.AddComponent<RenderComponent>
                            (worldObject,
                             AssetHandle<StaticMesh>(ASSETID_INVALID),
                             AssetHandle<ShaderProgram>(ShaderAsset_Lit_Inst_AlphaTest_NormalUp),
                             AssetHandle<Texture>(TextureAsset_Billboard_Grass),
                             MeshType::OrientedBillboard,
                             m_ParticleSystemMesh.GetID());

                    renderComponent.m_CanInstance = true;
                }

                ++spawnedCount;
            }

            zone.SetProceduralSpawningDone();
        }
    }
}

void SpawningHandler::HandleWorldEvent(WorldEvent _event)
{
    switch (_event.m_Type)
    {
    case WorldEvent::Type::AddTerrain:
    case WorldEvent::Type::RemoveTerrain:
    {
        const WorldPosition worldPosition = *_event.m_TargetPosition;
        const f32 radius = *_event.m_Radius;
        const f32 squareDistance = radius * radius;

        for (WorldZone& zone : m_World->GetActiveZones())
        {
            const glm::vec3 localPosition = worldPosition.GetPositionRelativeTo(zone);

            // TODO we will want a separate component to specify that a spawned object is attached to the terrain.
            for (ColliderComponent& collider : zone.GetComponents<ColliderComponent>())
            {
                if (collider.m_MovementType != MovementType::Fixed)
                {
                    continue;
                }

                WorldObject* owner = collider.GetOwnerObject();
                assert(owner != nullptr);

                const f32 squareDistanceToPoint = glm::length2(owner->GetPosition() - localPosition);

                if (squareDistanceToPoint < squareDistance)
                {
                    m_World->DestroyWorldObject(owner->GetWorldObjectID());
                }
            }
        }

        break;
    }
    default:
    break;
    }
}

void SpawningHandler::DebugDraw(UIDrawInterface& _interface) const
{
    for (WorldZone& zone : m_World->GetActiveZones())
    {
        u32 index = 0;
        for (ColliderComponent& collider : zone.GetComponents<ColliderComponent>())
        {
            const PlantInstance& plant = m_PlantInstances[index];

            index = (index + 1) % m_PlantInstances.size();
            
            glm::vec3 localPosition = collider.GetOwnerObject()->GetPosition();

            for (const PlantInstanceNode& node : plant.m_Nodes)
            {
                const glm::vec3 startPos = localPosition + node.m_RelativePosition;
                
                for (u32 childIdx = 0; childIdx < node.m_ChildCount; ++childIdx)
                {
                    const u32 childNodeIdx = node.m_Children[childIdx];
                    
                    const glm::vec3 endPos = localPosition + plant.m_Nodes[childNodeIdx].m_RelativePosition;

                    const f32 length = glm::length(endPos - startPos);
                    const glm::vec3 normal = (endPos - startPos) / length;

                    const f32 shading = static_cast<f32>(index) / static_cast<f32>(m_PlantInstances.size()); 
                    _interface.DebugDrawArrow(zone.GetCoordinates(), startPos, length, normal, Color(shading, 0.f, 0.f, 1.f));
                }
            }
        }
    }
}
