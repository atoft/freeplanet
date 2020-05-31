//
// Created by alastair on 31/05/2020.
//


#pragma once

class World;

// Deals with the spawning of procedural objects in the world.
class SpawningHandler
{
public:
    explicit SpawningHandler(World* _world);

    void Update();

private:
    World* m_World = nullptr;
};
