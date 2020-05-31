//
// Created by alastair on 31/05/2020.
//

#include "SpawningHandler.h"

#include <src/graphics/RenderComponent.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/World.h>

#include <random>
#include <src/tools/MathsHelpers.h>
#include <src/world/planet/PlanetGeneration.h>

SpawningHandler::SpawningHandler(World* _world)
    : m_World(_world)
{}

void SpawningHandler::Update()
{
    const Planet* planet = m_World->GetPlanet();

    if (planet == nullptr)
    {
        return;
    }

    for (WorldZone& zone : m_World->GetActiveZones())
    {
        // Use the terrain's RawMesh to allow us to query vertices directly.
        // This approach is not ideal since it prevents us from spawning objects in the same place in the vista as
        // in the actual world.
        const RawMesh& rawMesh = zone.GetTerrainComponent().m_DynamicMesh.GetRawMesh();

        if (!zone.IsProceduralSpawningDone() && !rawMesh.IsEmpty())
        {
            constexpr f32 fractionOfVertsToSpawnOn = 0.03125f;

            const Planet::Biome& biome = TerrainGeneration::GetBiome(*planet, WorldPosition(zone.GetCoordinates(), glm::vec3(0.f)));

            const u32 targetTreeQuantity = biome.m_TreeCoverage * fractionOfVertsToSpawnOn * rawMesh.m_Vertices.size();

            // TODO probably need to mutate the seed based on the zone to avoid an identical shuffle for every zone.
            std::mt19937 gen(planet->m_TerrainSeed);

            std::vector<glm::vec3> verts = rawMesh.m_Vertices;
            std::shuffle(verts.begin(), verts.end(), gen);

            u32 spawnedCount = 0;
            auto vertIt = verts.begin();
            while (spawnedCount < targetTreeQuantity && vertIt < verts.end())
            {
                const glm::vec3 chunksToZoneOriginOffset = glm::vec3(zone.GetTerrainComponent().m_ChunkSize * zone.GetTerrainComponent().m_ChunksPerEdge) /2.f;

                const glm::vec3 spawnPosition = *vertIt - chunksToZoneOriginOffset;

                ++vertIt;

                if (glm::any(glm::greaterThanEqual(glm::abs(spawnPosition), glm::vec3(TerrainConstants::WORLD_ZONE_SIZE / 2.f))))
                {
                    continue;
                }

                // TODO Verify if the position is safe to spawn at - surface normal, vertical space, horizontal space.

                WorldObject& worldObject = m_World->ConstructWorldObject(zone, "Tree");
                worldObject.SetInitialPosition(spawnPosition);

                const glm::mat3x3 rotationMatrix = MathsHelpers::GenerateRotationMatrix3x3FromUp(
                        PlanetGeneration::GetUpDirection(*m_World->GetPlanet(), worldObject.GetWorldPosition()));

                MathsHelpers::SetRotationPart(worldObject.GetZoneTransform(), rotationMatrix);

                zone.AddComponent<RenderComponent>
                        (worldObject,
                        AssetHandle<StaticMesh>(MeshAsset_Tree),
                        AssetHandle<ShaderProgram>(ShaderAsset_Lit_Textured),
                        AssetHandle<Texture>(TextureAsset_Tree));

                ColliderComponent& collider = zone.AddComponent<ColliderComponent>(worldObject, CollisionPrimitiveType::OBB, MovementType::Movable);
                collider.m_Bounds = AABB(glm::vec3(.8f, 4.f, .8f), glm::vec3(0.f, 2.f, 0.f));
                collider.m_MovementType = MovementType::Fixed;

                ++spawnedCount;
            }

            zone.SetProceduralSpawningDone();
        }
    }
}
