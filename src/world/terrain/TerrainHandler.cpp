//
// Created by alastair on 08/09/19.
//

#include "TerrainHandler.h"

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

            m_TerrainMeshUpdaters.Clear(zone.GetCoordinates());
        }

        // The component needs its mesh updating, request an update if possible.
        if (terrainComponent.m_DirtyRegion != std::nullopt)
        {
            const TerrainMeshUpdateParams params =
                    {
                        // TODO Would be nice if the params were already stored in this struct in TerrainComponent, would prevent having to rebuild it every time.
                        zone.GetCoordinates(),
                        m_World->GetPlanet(),

                        terrainComponent.m_Terrain,
                        terrainComponent.m_TerrainChunks,
                        {terrainComponent.m_ChunksPerEdge, terrainComponent.m_ChunkSize},
                        *terrainComponent.m_DirtyRegion,
                        m_NormalGenerationMethod
                    };

            const bool canUpdate = m_TerrainMeshUpdaters.RequestLoad(zone.GetCoordinates(), params);

            if (canUpdate)
            {
                terrainComponent.m_DirtyRegion = std::nullopt;
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
            const glm::vec3 zonePositionOffset = relativeZoneOffset * glm::vec3(terrainComponent.m_ChunkSize * terrainComponent.m_ChunksPerEdge);
            const glm::vec3 chunksToZoneOriginOffset = glm::vec3(terrainComponent.m_ChunkSize * terrainComponent.m_ChunksPerEdge) /2.f;

            const glm::vec3 localPosition = _event.m_TargetPosition->m_LocalPosition + chunksToZoneOriginOffset + zonePositionOffset;
            const f32 radius = *_event.m_Radius;

            const glm::ivec3 minRegion = glm::floor((localPosition - glm::vec3(2.f * radius)) / terrainComponent.m_ChunkSize);
            const glm::ivec3 maxRegion = glm::ceil((localPosition + glm::vec3(2.f * radius)) / terrainComponent.m_ChunkSize);

            const TerrainRegion region =
                {
                    glm::clamp(minRegion, glm::ivec3(), glm::ivec3(terrainComponent.m_ChunksPerEdge)),
                    glm::clamp(maxRegion, glm::ivec3(), glm::ivec3(terrainComponent.m_ChunksPerEdge))
                };

            if (GetTerrainRegionVolume(region) == 0)
            {
                LogMessage("TerrainEvent doesn't affect zone " + glm::to_string(zone.GetCoordinates()));
                continue;
            }

            if (_event.m_Type == WorldEvent::Type::AddTerrain)
            {
                terrainComponent.m_Terrain.m_AdditiveElements.push_back(SphereTerrainElement(localPosition, radius));
            }
            else // RemoveTerrain
            {
                terrainComponent.m_Terrain.m_SubtractiveElements.push_back(SphereTerrainElement(localPosition, radius));
            }

            LogMessage("Updated chunks in range " + glm::to_string(minRegion) + " - " + glm::to_string(maxRegion));

            terrainComponent.m_DirtyRegion = region;
        }
    }
    default:
        break;
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

        return zone->GetTerrainComponent().m_Terrain.GetDensity(localPosition);
    }

    LogWarning("Tried to get density of an unloaded zone (" + glm::to_string(_position.m_ZoneCoordinates) + ")");
    return 0.f;
}
