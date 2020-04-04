//
// Created by alastair on 28/09/19.
//

#include "VistaHandler.h"

#include <glm/gtc/matrix_transform.hpp>

#include <src/world/World.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/profiling/Profiler.h>

std::array<u32, 3> VistaHandler::VERTS_PER_EDGE_FOR_LOD = {24, 18, 18};
std::array<u32, 3> VistaHandler::ZONES_PER_EDGE_FOR_LOD = {3, 9, 27};

VistaHandler::VistaHandler(const World* _world)
    : m_World(_world)
{
}

glm::vec3 VistaHandler::GetGlobalSamplingOffset(glm::ivec3 _coords, u32 _lod) const
{
    assert(ZONES_PER_EDGE_FOR_LOD.size() > _lod);
    return glm::vec3(_coords) * static_cast<f32>(ZONES_PER_EDGE_FOR_LOD[_lod]) * TerrainConstants::WORLD_ZONE_SIZE;
}

glm::vec3 VistaHandler::GetGlobalPositionOffset(glm::ivec3 _coords, u32 _lod) const
{
    assert(ZONES_PER_EDGE_FOR_LOD.size() > _lod);
    return (glm::vec3(_coords) - 0.5f) * static_cast<f32>(ZONES_PER_EDGE_FOR_LOD[_lod]) * TerrainConstants::WORLD_ZONE_SIZE;
}

u32 VistaHandler::GetZonesOffsetFromCenter(u32 _lod) const
{
    assert(ZONES_PER_EDGE_FOR_LOD.size() > _lod);
    return ZONES_PER_EDGE_FOR_LOD[_lod] / 2;
}

TerrainProperties VistaHandler::GetPropertiesForLOD(u32 _lod) const
{
    TerrainProperties properties;

    assert(ZONES_PER_EDGE_FOR_LOD.size() > _lod);
    properties.m_ChunksPerEdge = VERTS_PER_EDGE_FOR_LOD[_lod];
    properties.m_ChunkSize = TerrainConstants::WORLD_ZONE_SIZE * ZONES_PER_EDGE_FOR_LOD[_lod] / VERTS_PER_EDGE_FOR_LOD[_lod];

    return properties;
}

void VistaHandler::Update(TimeMS _dt)
{
    ProfileCurrentFunction();

    if (m_World->GetPlanet() == nullptr)
    {
        return;
    }

    for (const VistaChunkIdentifier& identifier : m_TerrainMeshUpdaters.GetLoadedIdentifiers())
    {
        if (identifier.m_Generation == m_Generation)    // Hack, see comment in .h
        {
            RawMesh rawMesh = m_TerrainMeshUpdaters.Get(identifier)->GetRawMesh();

            const glm::vec3 globalOffset = GetGlobalPositionOffset(identifier.m_Coords, identifier.m_LOD);

            rawMesh.Translate(globalOffset);
            m_CombinedRawMesh.Append(rawMesh);
        }


        m_TerrainMeshUpdaters.Clear(identifier);
    }

    if (m_WaitingToUpdateDynamicMesh && m_PendingRequests.empty() && m_TerrainMeshUpdaters.IsAllLoaded())
    {
        m_DynamicMesh.RequestMeshUpdate(m_CombinedRawMesh);
        m_CombinedRawMesh = RawMesh();
        m_WaitingToUpdateDynamicMesh = false;
        m_WaitingToUpdateTransform = true;
    }

    if (m_WaitingToUpdateTransform && m_DynamicMesh.IsUpToDate())
    {
        m_TerrainModelTransform = glm::mat4(1.f);
        m_WaitingToUpdateTransform = false;
    }

    while (!m_PendingRequests.empty())
    {
        const VistaChunkRequest& request = m_PendingRequests.back();

        VistaChunkIdentifier identifier { request.m_LOD, request.m_Index, m_Generation };

        TerrainMeshUpdateParams params;
        params.m_Planet = m_World->GetPlanet();

        // TODO This provides incorrect zoneCoordinates, because there will be more than one zone inside each
        // TerrainMeshUpdate. Need more information inside the structure to do this properly.
        // Only works now for absolute positions/distances.
        params.m_ZoneCoordinates = request.m_Index * static_cast<s32>(ZONES_PER_EDGE_FOR_LOD[request.m_LOD]) + m_VistaOrigin;

        params.m_Properties = GetPropertiesForLOD(request.m_LOD);
        params.m_DirtyRegion = {glm::ivec3(), glm::ivec3(params.m_Properties.m_ChunksPerEdge)};
        params.m_ExistingChunks = std::vector<TerrainChunk>(params.m_Properties.m_ChunksPerEdge * params.m_Properties.m_ChunksPerEdge * params.m_Properties.m_ChunksPerEdge);
        params.m_Terrain = m_VistaTerrain;

        params.m_Terrain.m_GlobalPositionOffset = glm::vec3(m_VistaOrigin) * TerrainConstants::WORLD_ZONE_SIZE
                + GetGlobalSamplingOffset(request.m_Index, request.m_LOD)
                - glm::vec3(TerrainConstants::WORLD_ZONE_SIZE * GetZonesOffsetFromCenter(request.m_LOD)); // Because the origin of the Terrain is offset by (1 zone * 1 zone * 1 zone) in each direction, need to include this in the global offset so we get the same sampling.

        if (request.m_Index == glm::ivec3(0))
        {
            // Adjacent meshes of different LODs will have visible seams between them. Perfectly stitching the seams is
            // a non-trivial problem.
            // The current work-around is this. Still render the area covered by the active zones in the LOD0 mesh
            // (and still render the LOD0 area in the LOD1 mesh), but modify the terrain data itself so that this
            // area is heavily suppressed. This + ensuring the render order for the vista is always below the zones
            // themselves, should be *good enough* to cover up seams and holes at the zone boundaries.

            // TODO Keep this suppressed mesh separate so we can quickly remove it when zones change (otherwise you can see
            // the previously suppressed area).
            // Keep a cached LOD version of the currently active zones to swap in on demand.

            const f32 suppressScale = request.m_LOD == 0 ? 1.f : 3.f;

            params.m_Terrain.m_SubtractiveElements.push_back(BoxTerrainElement(glm::vec3(1.5f * suppressScale * TerrainConstants::WORLD_ZONE_SIZE), glm::vec3(1.3f * suppressScale * TerrainConstants::WORLD_ZONE_SIZE), 0.f));
        }

        const bool canUpdate = m_TerrainMeshUpdaters.RequestLoad(identifier, params);

        if (!canUpdate)
        {
            break;
        }

        m_PendingRequests.pop_back();
    }
}

void VistaHandler::OnLocalPlayerWorldZoneChanged(glm::ivec3 _zone)
{
    m_PendingRequests.clear();
    m_CombinedRawMesh = RawMesh();
    m_Generation++;

    if (m_World->GetPlanet() == nullptr)
    {
        return;
    }

    // We'll be waiting a while for the new vista to be generated for the player's new zone.
    // In the meantime, offset the existing vista so it's positioned correctly for the new zone.
    m_TerrainModelTransform = glm::translate(m_TerrainModelTransform, glm::vec3(m_VistaOrigin - _zone) * TerrainConstants::WORLD_ZONE_SIZE);

    m_VistaOrigin = _zone;

    for (u32 lod = 0; lod < LODS_IN_USE_COUNT; ++lod)
    {
        GenerateChunkRequestsForLOD(lod);
    }

    m_WaitingToUpdateDynamicMesh = true;
}

void VistaHandler::GenerateChunkRequestsForLOD(u32 _lod)
{
    m_PendingRequests.reserve(m_PendingRequests.size() + 27);

    for (s32 z = -1; z <= 1; ++z)
    {
        for (s32 y = -1; y <= 1; ++y)
        {
            for (s32 x = -1; x <= 1; ++x)
            {
                VistaChunkRequest chunkRequest;
                chunkRequest.m_LOD = _lod;
                chunkRequest.m_Index = glm::ivec3(x, y, z);

                m_PendingRequests.push_back(chunkRequest);
            }
        }
    }
}

glm::mat4x4 VistaHandler::GetTerrainModelTransform() const
{
    return m_TerrainModelTransform;
}
