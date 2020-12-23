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
