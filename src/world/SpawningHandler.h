//
// Created by alastair on 31/05/2020.
//


#pragma once

class World;
class WorldEvent;

// Deals with the spawning of procedural objects in the world.
class SpawningHandler
{
public:
    explicit SpawningHandler(World* _world);

    void Update();

    void HandleWorldEvent(WorldEvent _event);

private:
    World* m_World = nullptr;
};
