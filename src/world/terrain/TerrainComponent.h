//
// Created by alastair on 24/02/18.
//

#pragma once

#include <optional>
#include <vector>
#include <array>

#include <src/graphics/DynamicMeshHandle.h>
#include <src/tools/globals.h>
#include <src/world/GeometryTypes.h>
#include <src/world/terrain/Terrain.h>
#include <src/world/terrain/elements/TerrainElement.h>
#include <src/world/terrain/TerrainMeshUpdater.h>

class TerrainComponent
{
    friend class CollisionHandler;
    friend class TerrainHandler;

    // TODO This doesn't really make sense, need a rethink.
    friend class CollisionAlgorithms;

public:
    TerrainComponent(u32 _chunksPerEdge, f32 _chunkSize, glm::ivec3 _zoneCoords);
    TerrainComponent();

    u32 GetDimensions() const { return m_ChunksPerEdge; };

    // A bit of a hack, really only the TerrainHandler should be doing work on the TerrainComponent.
    // But until we have a proper idea of how worlds are generated, it's not clear who's in charge of
    // populating the procedural terrain. This will do for now...
    void ForceAddElement(const TerrainElementVariant& element);

    // For now we keep this accessible to be used by collision.
    // It wouldn't be too difficult to extract the triangles from the RawMesh and no longer use Chunks in physics.
    // Even then it may be worth caching them internally so that we don't have to regenerate all the chunks
    // in the TerrainMeshUpdater every time there's a change to only part of the Terrain.
    const std::vector<TerrainChunk>& GetChunks() const { return m_TerrainChunks; };

public:
    mutable DynamicMeshHandle m_DynamicMesh;

protected:
    Terrain m_Terrain;

    std::vector<TerrainChunk> m_TerrainChunks;

    u32 m_ChunksPerEdge = 0;
    f32 m_ChunkSize = 0.f;

    glm::ivec3 m_ZoneCoordinates;

    std::optional<TerrainRegion> m_DirtyRegion;
};
