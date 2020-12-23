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

#pragma once

#include <vector>

#include <src/tools/globals.h>
#include <src/world/Player.h>

class FreelookCameraComponent;
class World;
class WorldZone;

class PlayerHandler
{
public:
    explicit PlayerHandler(World* _world);

    void Update();

    void RegisterLocalPlayer(u32 _index);
    bool AreLocalPlayersSpawned() const;

    bool IsPlayerInZone(glm::ivec3 _coords) const;
    std::vector<WorldObjectID> GetLocalPlayers() const;
    bool IsControlledByLocalPlayer(WorldObjectID _id) const;
    const FreelookCameraComponent* GetLocalCamera() const;
    WorldObjectID GetControlledWorldObjectID(u32 _playerIndex) const;

private:
    enum class RequestState
    {
        WaitingToLoadZone,
        WaitingForZone,
        WaitingForTerrain
    };

    struct PlayerRequest
    {
        glm::ivec3 m_Zone = glm::ivec3();
        u32 m_Index = 0;
        RequestState m_State = RequestState::WaitingToLoadZone;
        u32 m_AttemptCount = 0;
    };

    bool SpawnPlayer(PlayerRequest& _request, WorldZone& _zone);
    glm::ivec3 GetZoneAbove(glm::ivec3 _zoneCoords) const;
        
private:
    World* m_World = nullptr;

    std::vector<PlayerRequest> m_Requests;
    std::vector<Player> m_LocalPlayers;
};



