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

#include "PlayerHandler.h"

#include <src/world/World.h>
#include <src/world/collision/CollisionHandler.h>
#include <src/world/terrain/TerrainHandler.h>
#include <src/world/planet/PlanetGeneration.h>
#include <src/world/terrain/TerrainConstants.h>

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

                const bool spawnSuccess = SpawnPlayer(request, *zone);

                if (spawnSuccess)
                {
                    m_Requests.erase(m_Requests.begin() + (requestIdx - 1));
                }

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

bool PlayerHandler::SpawnPlayer(PlayerRequest& _request, WorldZone& _zone)
{
    // Try to find a safe spawn point.
    glm::vec3 targetPosition = glm::vec3(0.f);

    const Planet* planet = m_World->GetPlanet();
    if (planet != nullptr)
    {
        const WorldPosition desiredWorldPosition = WorldPosition(_zone.GetCoordinates(), targetPosition);
        const glm::vec3 upDirection = PlanetGeneration::GetUpDirection(*planet, desiredWorldPosition);

        // Cast from a position on the sphere bounding the zone.
        const f32 radiusSqr = (TerrainConstants::WORLD_ZONE_SIZE / 2.f) * (TerrainConstants::WORLD_ZONE_SIZE / 2.f);
        const f32 raycastStartOffset = glm::sqrt(radiusSqr * 3.f);

        const glm::vec3 raycastStartPosition = upDirection * raycastStartOffset;

        std::optional<f32> raycastResult = m_World->GetCollisionHandler()->DoRaycast(
                desiredWorldPosition + (raycastStartPosition),
                -upDirection,
                CollisionHandler::RaycastRange::AllLoadedZones);

        if (raycastResult.has_value())
        {
            // Player origin is at center, not at feet, probably want to change that.
            targetPosition = raycastStartPosition -upDirection * (raycastResult.value() - 1.f);
        }

        else
        {
            constexpr u32 SPAWN_MAX_ATTEMPTS = 2;
            
            if (_request.m_AttemptCount < SPAWN_MAX_ATTEMPTS)
            {
                LogWarning("Couldn't find a free space to spawn at.");

                _request.m_Zone = GetZoneAbove(_zone.GetCoordinates());
                _request.m_State = RequestState::WaitingToLoadZone;
                ++_request.m_AttemptCount;
                return false;
            }
            else
            {
                LogError("Player spawning exceeded max attempt count.");
            }
        }
    }

    WorldObject& worldObject = m_World->ConstructWorldObject(_zone, "Player");
    worldObject.SetInitialPosition(targetPosition);

    _zone.AddComponent<BipedComponent>(worldObject);
    _zone.AddComponent<FreelookCameraComponent>(worldObject);

    ColliderComponent& collider = _zone.AddComponent<ColliderComponent>(worldObject, CollisionPrimitiveType::OBB, MovementType::Movable);
    collider.m_Bounds = AABB(glm::vec3(.5f, 1.f, .5f));
    collider.m_KeepUpright = true;

    Player player;
    player.AttachWorldObject(worldObject.GetWorldObjectID());
    m_LocalPlayers.push_back(player);

    m_World->OnPlayerSpawned(worldObject);
    return true;
}

glm::ivec3 PlayerHandler::GetZoneAbove(glm::ivec3 _zoneCoords) const
{
    // TODO Real implementation, find an adjacent zone in the up direction.
    return _zoneCoords + glm::ivec3(0, 1, 0);
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

WorldObjectID PlayerHandler::GetControlledWorldObjectID(u32 _playerIndex) const
{
    // TODO How do indexes work for non local players?

    if (m_LocalPlayers.size() <= _playerIndex)
    {
        return WORLDOBJECTID_INVALID;
    }

    return m_LocalPlayers[_playerIndex].GetControlledWorldObjectID();
}
