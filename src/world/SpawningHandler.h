//
// Created by alastair on 31/05/2020.
//


#pragma once

#include <vector>

#include <src/graphics/DynamicMeshHandle.h>
#include <src/world/particles/ParticleSystemComponent.h>
#include <src/world/flora/FloraGeneration.h>

class UIDrawInterface;
class World;
class WorldEvent;

// Deals with the spawning of procedural objects in the world.
class SpawningHandler
{
public:
    explicit SpawningHandler(World* _world);

    void Update();

    void HandleWorldEvent(WorldEvent _event);

    void DebugDraw(UIDrawInterface& _interface) const;

    mutable std::vector<DynamicMeshHandle> m_SpawnedPlantMeshes;
    mutable DynamicMeshHandle m_ParticleSystemMesh;
    std::vector<ParticleSystem> m_SpawnedParticleSystems;
    
private:
    World* m_World = nullptr;

    std::vector<PlantInstance> m_PlantInstances;
};
