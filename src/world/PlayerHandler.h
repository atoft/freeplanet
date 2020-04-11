//
// Created by alastair on 10/04/2020.
//

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
    };

    void SpawnPlayer(const PlayerRequest& _request, WorldZone& _zone);

private:
    World* m_World = nullptr;

    std::vector<PlayerRequest> m_Requests;
    std::vector<Player> m_LocalPlayers;
};



