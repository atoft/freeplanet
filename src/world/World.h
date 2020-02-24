//
// Created by alastair on 26/08/17.
//

#pragma once

#include <src/world/environment/EnvironmentState.h>
#include <src/world/events/WorldEvent.h>
#include <src/engine/events/EventHandler.h>
#include <src/engine/loader/DynamicLoaderCollection.h>
#include <src/engine/events/EngineEvent.h>
#include <src/engine/StartupHelpers.h>
#include <src/world/WorldZone.h>
#include <src/world/WorldObjectDirectory.h>
#include <src/world/Player.h>
#include <src/world/WorldZoneRegion.h>

class WorldObject;
class WorldZone;
class FreelookCameraComponent;
class CollisionHandler;
class TerrainHandler;
class VistaHandler;

// A World represents an environment in which gameplay occurs.
class World
{
public:
    World(std::string _worldName, u32 _seed, bool _initTerrain = false);

    void Update(TimeMS _delta);

    const WorldObject* GetWorldObject(WorldObjectID _objectID) const;
    WorldObject* GetWorldObject(WorldObjectID _objectID);
    void DestroyWorldObject(WorldObjectID _objectID);

    WorldZone* FindZoneAtCoordinates(glm::ivec3 _zoneCoordinates);
    const WorldZone* FindZoneAtCoordinates(glm::ivec3 _zoneCoordinates) const;

    std::vector<WorldZone>& GetActiveZones() { return m_ActiveZones; }
    const std::vector<WorldZone>& GetActiveZones() const { return m_ActiveZones; }

    void SpawnPlayerInWorldZone(glm::ivec3 _zoneCoordinates);
    void SpawnPropInWorldZone(const WorldPosition& _worldPosition, const PropRecipe& _propRecipe);

    std::string GetName();

    void OnButtonInput(InputType _inputType);
    void OnMouseInput(f32 _mouseX, f32 _mouseY);

    // TODO Neither of these should be public - this is because WorldObject management is not being consistently owned,
    // instead it's spread between World and WorldZone. This needs to be fixed, but is also a bit dependent on how
    // WorldObjects will generally be created outside of debugging.
    WorldObjectID RegisterWorldObject(WorldObjectRef _objectRef) { return m_Directory.RegisterWorldObject(_objectRef); };
    void OnWorldObjectTransferred(WorldObjectID _objectID, WorldObjectRef _newLocation) { m_Directory.OnWorldObjectTransferred(_objectID, _newLocation); };

    CollisionHandler* GetCollisionHandler() { return m_CollisionHandler.get(); };
    const CollisionHandler* GetCollisionHandler() const { return m_CollisionHandler.get(); };
    TerrainHandler* GetTerrainHandler() { return m_TerrainHandler.get(); };
    const TerrainHandler* GetTerrainHandler() const { return m_TerrainHandler.get(); };
    const VistaHandler* GetVistaHandler() const { return m_VistaHandler.get(); };

    void HandleEvent(EngineEvent _event);
    void HandleCommandLineArgs(const CommandLineArgs& _args);

    void AddWorldEvent(WorldEvent _event);

    void DebugDraw(UIDrawInterface& _interface) const;

    bool IsPlayerInZone(glm::ivec3 _coords) const;
    std::vector<WorldObjectID> GetLocalPlayers() const;

    const EnvironmentState& GetEnvironmentState() const { return m_EnvironmentState; };
    const WorldZoneRegion& GetWorldBounds() const { return m_WorldBounds; };
    f32 GetTimeScale() const { return m_TimeScale; };
    f32 GetGravityStrength() const { return m_GravityStrength; };

private:
    const WorldObject* FindWorldObject(const WorldObjectRef& _objectRef) const;

    void TransferEntitiesBetweenZones();
    void UpdateActiveZones();
    void SendWorldEvents();

    bool IsZoneLoading(glm::ivec3 _coords) const;
    bool IsZoneLoaded(glm::ivec3 _coords) const;
    void LoadZone(World* _world, glm::ivec3 _position, glm::vec3 _dimensions, u32 _terrainSeed, bool _initTerrain);

private:
    std::vector<WorldZone> m_ActiveZones;

    static constexpr u32 CONCURRENT_WORLD_ZONE_LOADERS_COUNT = 9;
    using WorldZoneLoaders = DynamicLoaderCollection<WorldZone, glm::ivec3, CONCURRENT_WORLD_ZONE_LOADERS_COUNT, World*, glm::ivec3, glm::vec3, u32, bool>;
    WorldZoneLoaders m_ZoneLoaders;

    u32 m_TerrainSeed = 0;
    // TODO Remove once we have a pipeline for terrain generation.
    bool m_ShouldInitTerrain = false;

    std::string m_Name;
    WorldZoneRegion m_WorldBounds;

    WorldObjectDirectory m_Directory;

    std::vector<Player> m_ActivePlayers;

    std::shared_ptr<CollisionHandler> m_CollisionHandler;
    std::shared_ptr<TerrainHandler> m_TerrainHandler;
    std::shared_ptr<VistaHandler> m_VistaHandler;

    EventHandler<WorldEvent> m_WorldEventHandler;

    EnvironmentState m_EnvironmentState;

    f32 m_TimeScale = 1.f;
    f32 m_GravityStrength = 9.81f;
};
