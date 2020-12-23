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

        switch (request.m_LOD)
        {
        case 0:
            params.m_LevelOfDetail = TerrainLevelOfDetail::NearVista;
            break;
        case 1:
            params.m_LevelOfDetail = TerrainLevelOfDetail::FarVista;
            break;
        case 2:
            params.m_LevelOfDetail = TerrainLevelOfDetail::Planetary;
            break;
        default:
            break;
        }
        static_assert(LODS_IN_USE_COUNT == 3);

        params.m_IsFringe = request.m_IsFringe;

        if (request.m_IsFringe)
        {
            // Adjacent meshes of different LODs will have visible seams between them. Perfectly stitching the seams is
            // a non-trivial problem.
            // The current work-around is this. Render a "fringe" around the edge of the active zones in the lower LOD mesh,
            // and decrease the density of the terrain towards the inside of the fringe. This gives a nice overlap with
            // less change of gaps between the meshes.

            // TODO Keep this suppressed mesh separate so we can quickly remove it when zones change (otherwise you can see
            // the previously suppressed area).
            // Keep a cached LOD version of the currently active zones to swap in on demand.

            const f32 suppressScale = glm::pow(3.f, request.m_LOD);

            params.m_TerrainEdits.m_SubtractiveElements.push_back(BoxTerrainElement(glm::vec3(1.5f * suppressScale * TerrainConstants::WORLD_ZONE_SIZE), glm::vec3(1.4f * suppressScale * TerrainConstants::WORLD_ZONE_SIZE), 0.f));
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

                if (x == 0 && y == 0 && z == 0)
                {
                    chunkRequest.m_IsFringe = true;
                }

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
