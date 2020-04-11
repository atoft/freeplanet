//
// Created by alastair on 10/04/2020.
//

#include "PlayerHandler.h"

#include <src/world/World.h>
#include <src/world/terrain/TerrainHandler.h>

PlayerHandler::PlayerHandler(World* _world)
    : m_World(_world)
{}


void PlayerHandler::Update()
{
    for (u32 requestIdx = m_Requests.size(); requestIdx > 0; --requestIdx)
    {
        PlayerRequest& request = m_Requests[requestIdx - 1];

        switch (request.m_State)
        {
        case RequestState::WaitingToLoadZone:
        {
            const bool isLoading = m_World->TryLoadZone(request.m_Zone);

            if (isLoading)
            {
                request.m_State = RequestState::WaitingForZone;
            }
            break;
        }
        case RequestState::WaitingForZone:
        {
            for (WorldZone& zone : m_World->GetActiveZones())
            {
                if (zone.GetCoordinates() == request.m_Zone)
                {
                    request.m_State = RequestState::WaitingForTerrain;
                    break;
                }
            }

            break;
        }
        case RequestState ::WaitingForTerrain:
        {
            const bool isTerrainLoaded = m_World->GetTerrainHandler()->IsAllLoaded();

            if (isTerrainLoaded)
            {
                for (WorldZone& zone : m_World->GetActiveZones())
                {
                    if (zone.GetCoordinates() == request.m_Zone)
                    {
                        SpawnPlayer(request, zone);

                        m_Requests.erase(m_Requests.begin() + (requestIdx - 1));
                    }

                    break;
                }
            }

            break;
        }
        default:
            break;
        }
    }
}

void PlayerHandler::RegisterLocalPlayer(u32 _index)
{
    glm::ivec3 spawnZone = glm::ivec3(0, 0, 0);

    if (m_World->GetPlanet() != nullptr)
    {
        // TODO get from planet.
        spawnZone = glm::ivec3(0, 32, 0);
    }
    
    m_Requests.push_back({spawnZone, _index, RequestState::WaitingToLoadZone});
}

bool PlayerHandler::AreLocalPlayersSpawned() const
{
    return m_Requests.empty() && !m_World->GetLocalPlayers().empty();
}

void PlayerHandler::SpawnPlayer(const PlayerRequest& _request, WorldZone& _zone)
{
    // TODO handle the spawning here.
    // TODO find a safe space to spawn in.
    m_World->SpawnPlayerInWorldZone(_zone.GetCoordinates());
}
