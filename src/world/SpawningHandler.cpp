//
// Created by alastair on 31/05/2020.
//

#include "SpawningHandler.h"

#include <random>
#include <glm/gtx/norm.hpp>

#include <src/graphics/RenderComponent.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/World.h>
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

    // TODO: Actually, this happens per biome, and we need to be smart about when it occurs.
    if (m_PlantInstances.empty())
    {
        m_PlantInstances.push_back(FloraGeneration::GeneratePlant(FloraGenerationParams(), 0));

        // TODO Request generation of the mesh.

        // Generate the leaves.
    }

    for (WorldZone& zone : m_World->GetActiveZones())
    {
        // Use the terrain's RawMesh to allow us to query vertices directly.
        // This approach is not ideal since it prevents us from spawning objects in the same place in the vista as
        // in the actual world.
        const RawMesh& rawMesh = zone.GetTerrainComponent().m_DynamicMesh.GetRawMesh();

        if (!zone.IsProceduralSpawningDone() && !rawMesh.IsEmpty())
        {
            constexpr f32 fractionOfVertsToSpawnTreesOn = 0.03125f;
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

//                    zone.AddComponent<RenderComponent>
//                            (worldObject,
//                             AssetHandle<StaticMesh>(MeshAsset_Tree),
//                             AssetHandle<ShaderProgram>(ShaderAsset_Lit_Textured),
//                             AssetHandle<Texture>(TextureAsset_Tree),
//                             MeshType::Normal);

                    ColliderComponent& collider = zone.AddComponent<ColliderComponent>(worldObject,
                                                                                       CollisionPrimitiveType::OBB,
                                                                                       MovementType::Movable);
                    collider.m_Bounds = AABB(glm::vec3(.8f, 4.f, .8f), glm::vec3(0.f, 2.f, 0.f));
                    collider.m_MovementType = MovementType::Fixed;
                }
                else
                {
                    // Probably(?) don't want to go as far as having a WorldObject for every blade of grass. Probably some
                    // simpler structure which caches all the points to draw a sprite at.
                    WorldObject &worldObject = m_World->ConstructWorldObject(zone, "Grass");
                    worldObject.SetInitialPosition(spawnPosition + glm::vec3(0.f, 0.5f, 0.f));

                    const glm::mat3x3 rotationMatrix = MathsHelpers::GenerateRotationMatrix3x3FromUp(
                            PlanetGeneration::GetUpDirection(*m_World->GetPlanet(), worldObject.GetWorldPosition()));

                    MathsHelpers::SetRotationPart(worldObject.GetZoneTransform(), rotationMatrix);

                    zone.AddComponent<RenderComponent>
                            (worldObject,
                             AssetHandle<StaticMesh>(MeshAsset_UnitQuad),
                             AssetHandle<ShaderProgram>(ShaderAsset_Lit_AlphaTest_NormalUp),
                             AssetHandle<Texture>(TextureAsset_Billboard_Grass),
                             MeshType::OrientedBillboard);
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
        for (ColliderComponent& collider : zone.GetComponents<ColliderComponent>())
        {
            const PlantInstance& plant = m_PlantInstances[0];

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

                    _interface.DebugDrawArrow(zone.GetCoordinates(), startPos, length, normal);
                }
            }
        }
    }
}
