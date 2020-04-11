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
            const bool isLoadingOrLoaded = m_World->RequestZone(request.m_Zone);

            if (isLoadingOrLoaded)
            {
                request.m_State = RequestState::WaitingForZone;
            }
            break;
        }
        case RequestState::WaitingForZone:
        {
            if (m_World->FindZoneAtCoordinates(request.m_Zone) != nullptr)
            {
                request.m_State = RequestState::WaitingForTerrain;
            }
            break;
        }
        case RequestState ::WaitingForTerrain:
        {
            const bool isTerrainLoaded = m_World->GetTerrainHandler()->IsAllLoaded();

            if (isTerrainLoaded)
            {
                WorldZone* zone = m_World->FindZoneAtCoordinates(request.m_Zone);

                // We might hit this once there are multiple players. In that case need to fix World
                // to not unload zones if there's a player requesting them.
                assert(zone != nullptr);

                SpawnPlayer(request, *zone);

                m_Requests.erase(m_Requests.begin() + (requestIdx - 1));

                break;
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
    return m_Requests.empty() && !m_LocalPlayers.empty();
}

void PlayerHandler::SpawnPlayer(const PlayerRequest& _request, WorldZone& _zone)
{
    WorldObject& worldObject = m_World->ConstructWorldObject(_zone, "Player");
    _zone.AddComponent<BipedComponent>(worldObject);
    _zone.AddComponent<FreelookCameraComponent>(worldObject);

    ColliderComponent& collider = _zone.AddComponent<ColliderComponent>(worldObject, CollisionPrimitiveType::OBB, MovementType::Movable);
    collider.m_Bounds = glm::vec3(.5f, 1.f, .5f);
    collider.m_KeepUpright = true;

    Player player;
    player.AttachWorldObject(worldObject.GetWorldObjectID());
    m_LocalPlayers.push_back(player);

    m_World->OnPlayerSpawned(worldObject);
}

bool PlayerHandler::IsPlayerInZone(glm::ivec3 _coords) const
{
    for (const Player& player : m_LocalPlayers)
    {
        WorldObjectID playerControlledObjectID = player.GetControlledWorldObjectID();

        if (playerControlledObjectID != WORLDOBJECTID_INVALID)
        {
            return m_World->LocateWorldObject(playerControlledObjectID) == _coords;
        }
    }

    return false;
}

std::vector<WorldObjectID> PlayerHandler::GetLocalPlayers() const
{
    std::vector<WorldObjectID> result;

    for (const Player& player : m_LocalPlayers)
    {
        if (player.m_IsLocal)
        {
            result.push_back(player.GetControlledWorldObjectID());
        }
    }
    return result;
}

bool PlayerHandler::IsControlledByLocalPlayer(WorldObjectID _id) const
{
    for (const Player& player : m_LocalPlayers)
    {
        if (player.m_IsLocal && player.GetControlledWorldObjectID() == _id)
        {
            return true;
        }
    }

    return false;
}

const FreelookCameraComponent* PlayerHandler::GetLocalCamera() const
{
    if (m_LocalPlayers.empty())
    {
        return nullptr;
    }

    // To support splitscreen, return a list of cameras here.
    const WorldObject* worldObject = m_World->GetWorldObject(m_LocalPlayers[0].GetControlledWorldObjectID());

    if (worldObject == nullptr)
    {
        return nullptr;
    }

    // To support players using a remote camera, add a controlled camera ID to the player struct.
    return ComponentAccess::GetComponent<FreelookCameraComponent>(*worldObject);
}
