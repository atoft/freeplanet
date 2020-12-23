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

    bool IsAllLoaded() const;

private:
    void HandleFinishedUpdaterFeedback(WorldZone& _zone, const TerrainMeshUpdater* _finishedUpdater);
    
    World* m_World = nullptr;

    static constexpr u32 CONCURRENT_TERRAIN_UPDATERS_COUNT = 3;
    using TerrainMeshUpdaters = DynamicLoaderCollection<TerrainMeshUpdater, glm::ivec3, CONCURRENT_TERRAIN_UPDATERS_COUNT, TerrainMeshUpdateParams>;
    TerrainMeshUpdaters m_TerrainMeshUpdaters;

    NormalGenerationMethod m_NormalGenerationMethod = NormalGenerationMethod::FromVolume;
};
