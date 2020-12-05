//
// Created by alastair on 08/09/19.
//

#include "TerrainHandler.h"
#include "src/assets/MeshAssets.h"
#include "src/assets/ShaderAssets.h"
#include "src/assets/TextureAssets.h"
#include "src/world/SpawningHandler.h"
#include "src/world/events/WorldEvent.h"

#include <src/world/collision/CollisionHandler.h>
#include <src/world/planet/PlanetGeneration.h>
#include <src/world/World.h>
#include <src/world/WorldZone.h>
#include <src/tools/MathsHelpers.h>
#include <src/profiling/Profiler.h>

TerrainHandler::TerrainHandler(World* _world)
    : m_World(_world) {}

void TerrainHandler::Update(TimeMS _dt)
{
    ProfileCurrentFunction();

    for (WorldZone& zone : m_World->GetActiveZones())
    {
        TerrainComponent& terrainComponent = zone.GetTerrainComponent();

        const TerrainMeshUpdater* finishedUpdater = m_TerrainMeshUpdaters.Get(zone.GetCoordinates());

        // The updater has generated new mesh and chunk data, so copy into the actual component.
        if (finishedUpdater != nullptr)
        {
            terrainComponent.m_TerrainChunks = finishedUpdater->GetChunks();

            // Trigger the renderer to make a new DynamicMesh for this TerrainComponent.
            terrainComponent.m_DynamicMesh.RequestMeshUpdate(finishedUpdater->GetRawMesh());

            HandleFinishedUpdaterFeedback(zone, finishedUpdater);
 
            m_TerrainMeshUpdaters.Clear(zone.GetCoordinates());
        }

        // The component needs its mesh updating, request an update if possible.
        if (terrainComponent.m_DirtyRegion != std::nullopt)
        {
            const TerrainMeshUpdateParams params =
                    {
                        zone.GetCoordinates(),
                        m_World->GetPlanet(),

                        terrainComponent.m_TerrainEdits,
                        terrainComponent.m_TerrainChunks,
                        terrainComponent.m_Properties,
                        *terrainComponent.m_DirtyRegion,
                        m_NormalGenerationMethod,
                        TerrainLevelOfDetail::ActiveZone,
                        false,
                        terrainComponent.m_AssociatedEvents
                    };
            
            const bool canUpdate = m_TerrainMeshUpdaters.RequestLoad(zone.GetCoordinates(), params);

            if (canUpdate)
            {
                terrainComponent.m_DirtyRegion = std::nullopt;
                terrainComponent.m_AssociatedEvents.clear();
            }
        }
    }

    // It's possible that while we were updating a TerrainMesh, the zone it belonged to was unloaded.
    // In that case we need to discard it manually.
    for (const glm::ivec3& zoneCoords : m_TerrainMeshUpdaters.GetLoadedIdentifiers())
    {
        if (m_World->FindZoneAtCoordinates(zoneCoords) == nullptr)
        {
            m_TerrainMeshUpdaters.Clear(zoneCoords);
        }
    }
}

u32 GetTerrainRegionVolume(const TerrainRegion& _region)
{
    glm::ivec3 regionDimensions = _region.m_Max - _region.m_Min;

    return glm::max(0, regionDimensions.x * regionDimensions.y *regionDimensions.z);
}

void TerrainHandler::HandleWorldEvent(WorldEvent _event)
{
    switch (_event.m_Type)
    {
    case WorldEvent::Type::AddTerrain:
    case WorldEvent::Type::RemoveTerrain:
    {
        for (WorldZone& zone : m_World->GetActiveZones())
        {
            TerrainComponent& terrainComponent = zone.GetTerrainComponent();

            const glm::vec3 relativeZoneOffset = _event.m_TargetPosition->m_ZoneCoordinates - zone.GetCoordinates();
            const glm::vec3 zonePositionOffset = relativeZoneOffset * glm::vec3(terrainComponent.m_Properties.m_ChunkSize * terrainComponent.m_Properties.m_ChunksPerEdge);

            const glm::vec3 localPosition = TerrainHelpers::ToTerrainMeshSpace(_event.m_TargetPosition->m_LocalPosition, terrainComponent.m_Properties) + zonePositionOffset;
            const f32 radius = *_event.m_Radius;

            const glm::ivec3 minRegion = glm::floor((localPosition - glm::vec3(2.f * radius)) / terrainComponent.m_Properties.m_ChunkSize);
            const glm::ivec3 maxRegion = glm::ceil((localPosition + glm::vec3(2.f * radius)) / terrainComponent.m_Properties.m_ChunkSize);

            const TerrainRegion region =
                {
                    glm::clamp(minRegion, glm::ivec3(), glm::ivec3(terrainComponent.m_Properties.m_ChunksPerEdge)),
                    glm::clamp(maxRegion, glm::ivec3(), glm::ivec3(terrainComponent.m_Properties.m_ChunksPerEdge))
                };

            if (GetTerrainRegionVolume(region) == 0)
            {
                continue;
            }

            if (_event.m_Type == WorldEvent::Type::AddTerrain)
            {
                terrainComponent.m_TerrainEdits.m_AdditiveElements.push_back(SphereTerrainElement(localPosition, radius, *_event.m_Substance));
            }
            else if (_event.m_Type == WorldEvent::Type::RemoveTerrain)
            {
                terrainComponent.m_TerrainEdits.m_SubtractiveElements.push_back(SphereTerrainElement(localPosition, radius, TerrainSubstanceType::Count));
            }

            LogMessage("Updated chunks in range " + glm::to_string(minRegion) + " - " + glm::to_string(maxRegion));

            if (terrainComponent.m_DirtyRegion != std::nullopt)
            {
                // @Performance Could support a list of regions and figure out the min chunks to update to cover them.
                terrainComponent.m_DirtyRegion = terrainComponent.m_DirtyRegion->Union(region);
            }
            else
            {
                terrainComponent.m_DirtyRegion = region;
            }
            
            terrainComponent.m_AssociatedEvents.push_back(_event);
        }
    }
    default:
        break;
    }
}

void TerrainHandler::HandleFinishedUpdaterFeedback(WorldZone& _zone, const TerrainMeshUpdater* _finishedUpdater)
{
    for (const WorldEvent& event : _finishedUpdater->GetAssociatedEvents())
    {
        if (event.m_Type == WorldEvent::Type::AddTerrain)
        {
            AABB eventBounds;
            eventBounds.m_Dimensions = glm::vec3(*event.m_Radius);

            std::vector<WorldObjectID> overlaps = m_World->GetCollisionHandler()->DoShapecast(*event.m_TargetPosition, eventBounds);

            for (const WorldObjectID& overlapID : overlaps)
            {
                WorldObject* overlappedObject = m_World->GetWorldObject(overlapID);

                if (overlappedObject == nullptr)
                {
                    continue;
                }

                const glm::vec3 objectLocalPosition = overlappedObject->GetPosition();
                const glm::vec3 eventPosition = event.m_TargetPosition->m_LocalPosition;

                const glm::vec3 eventToObject = objectLocalPosition - eventPosition;
                const f32 distanceFromEvent = glm::length(eventToObject);

                const f32 correctionDistance = *event.m_Radius - distanceFromEvent;

                constexpr f32 OVERLAPPED_OBJECT_RADIUS = 1.f;

                if (correctionDistance > 0.f)
                {
                    const glm::vec3 correctionVector = (eventToObject / distanceFromEvent) * (correctionDistance + OVERLAPPED_OBJECT_RADIUS);
                    overlappedObject->SetPosition(overlappedObject->GetPosition() + correctionVector);
                }
            }
        }
        else if (event.m_Type == WorldEvent::Type::RemoveTerrain)
        {
            if (event.m_TargetPosition->m_ZoneCoordinates != _zone.GetCoordinates())
            {
                continue;
            }
            
            WorldObject& worldObject = m_World->ConstructWorldObject(_zone, "TerrainRemovedParticles");
            worldObject.SetInitialPosition(event.m_TargetPosition->m_LocalPosition);

            const glm::mat3x3 rotationMatrix = MathsHelpers::GenerateRotationMatrix3x3FromUp(
                PlanetGeneration::GetUpDirection(*m_World->GetPlanet(), *event.m_TargetPosition));

            MathsHelpers::SetRotationPart(worldObject.GetZoneTransform(), rotationMatrix);

            
            ParticleSystemComponent& particleSystem =_zone.AddComponent<ParticleSystemComponent>(worldObject);
            particleSystem.m_ShouldDestroyOwnerOnFinish = true;
            
            ParticleEmitter& emitter = particleSystem.m_ParticleSystem.m_Emitters.emplace_back();

            // TODO Address this in mesh refactor.
            emitter.m_MeshID = m_World->GetSpawningHandler()->m_ParticleSystemMesh.GetID();
            emitter.m_Texture = AssetHandle<Texture>(TextureAsset_Billboard_DirtParticle);
            emitter.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Lit_Inst_AlphaBlend_NormalUp);
            emitter.m_NeedsDepthSort = true;
            emitter.m_UseParticleColor = true;
            
            emitter.m_EmissionRate = 0.1f;
            emitter.m_ParticlesPerEmission = 6;
            emitter.m_InitialSpeed = 2.f;
            emitter.m_Acceleration = m_World->GetGravityStrength() * glm::vec3(0.f, -1.f, 0.f);
            emitter.m_ParticleLifetime = 0.6f;
            emitter.m_FadeoutDuration = 0.3f;
            emitter.m_EmitterLifetime = 1.f;
        }
    }
}

// TODO This is now invalid, as it's not accounting for Planet density.
f32 TerrainHandler::GetDensity(const WorldPosition& _position) const
{
    const WorldZone* zone = m_World->FindZoneAtCoordinates(_position.m_ZoneCoordinates);

    if (zone != nullptr)
    {
        const glm::vec3 terrainPositionOffset = MathsHelpers::GetPosition(zone->GetTerrainModelTransform());
        const glm::vec3 localPosition = _position.m_LocalPosition - terrainPositionOffset;

        return zone->GetTerrainComponent().m_TerrainEdits.GetDensity(localPosition);
    }

    LogWarning("Tried to get density of an unloaded zone (" + glm::to_string(_position.m_ZoneCoordinates) + ")");
    return 0.f;
}

bool TerrainHandler::IsAllLoaded() const
{
    for (const WorldZone& zone : m_World->GetActiveZones())
    {
        if (zone.GetTerrainComponent().m_DirtyRegion != std::nullopt)
        {
            // There's an update we haven't handled.
            return false;
        }
    }

    // Is there an update in flight?
    return m_TerrainMeshUpdaters.IsEmpty();
}
