//
// Created by alastair on 10/04/2020.
//

#pragma once

#include <vector>

#include <src/tools/globals.h>

class World;
class WorldZone;

class PlayerHandler
{
public:
    explicit PlayerHandler(World* _world);

    void Update();

    void RegisterLocalPlayer(u32 _index);
    bool AreLocalPlayersSpawned() const;

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
};



