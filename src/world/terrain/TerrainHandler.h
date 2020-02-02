//
// Created by alastair on 08/09/19.
//


#pragma once

#include <src/tools/globals.h>
#include <src/engine/loader/DynamicLoaderCollection.h>
#include <src/world/events/WorldEvent.h>
#include <src/world/terrain/TerrainMeshUpdater.h>

class World;

class TerrainHandler
{
public:
    explicit TerrainHandler(World* _world);

    void Update(TimeMS _dt);
    void HandleWorldEvent(WorldEvent _event);

    void SetNormalGenerationMethod(NormalGenerationMethod _method) { m_NormalGenerationMethod = _method; };

    f32 GetDensity(const WorldPosition& _position) const;

private:
    World* m_World = nullptr;

    static constexpr u32 CONCURRENT_TERRAIN_UPDATERS_COUNT = 3;
    using TerrainMeshUpdaters = DynamicLoaderCollection<TerrainMeshUpdater, glm::ivec3, CONCURRENT_TERRAIN_UPDATERS_COUNT, TerrainMeshUpdateParams>;
    TerrainMeshUpdaters m_TerrainMeshUpdaters;

    NormalGenerationMethod m_NormalGenerationMethod = NormalGenerationMethod::FromVolume;
};
