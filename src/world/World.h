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
#include <src/world/ComponentAccess.h>
#include <src/world/PlayerHandler.h>
#include <src/world/WorldZone.h>
#include <src/world/WorldObjectDirectory.h>
#include <src/world/Player.h>
#include <src/world/WorldZoneRegion.h>
#include <src/world/planet/Planet.h>

class WorldObject;
class WorldZone;
class FreelookCameraComponent;
class CollisionHandler;
class SpawningHandler;
class TerrainHandler;
class VistaHandler;
class InventoryHandler;
class ParticleSystemHandler;

// A World represents an environment in which gameplay occurs.
class World
{
public:
    World(std::string _worldName, std::optional<Planet> _planet);

    void Update(TimeMS _delta);

    const WorldObject* GetWorldObject(WorldObjectID _objectID) const;
    WorldObject* GetWorldObject(WorldObjectID _objectID);

    glm::ivec3 LocateWorldObject(WorldObjectID _objectID) const;

    WorldObject& ConstructWorldObject(WorldZone& _zone, const std::string& _name);
    void DestroyWorldObject(WorldObjectID _objectID);

    WorldZone* FindZoneAtCoordinates(glm::ivec3 _zoneCoordinates);
    const WorldZone* FindZoneAtCoordinates(glm::ivec3 _zoneCoordinates) const;

    std::vector<WorldZone>& GetActiveZones() { return m_ActiveZones; }
    const std::vector<WorldZone>& GetActiveZones() const { return m_ActiveZones; }

    void RegisterLocalPlayer(u32 _playerIndex);

    void SpawnPropInWorldZone(const WorldPosition& _worldPosition, const PropRecipe& _propRecipe);

    std::string GetName();

    void OnButtonInput(InputType _inputType);
    void OnMouseInput(f32 _mouseX, f32 _mouseY);

    void OnWorldObjectTransferred(WorldObjectID _objectID, WorldObjectRef _newLocation) { m_Directory.OnWorldObjectTransferred(_objectID, _newLocation); };

    void OnPlayerSpawned(const WorldObject& _controlledObject);

    CollisionHandler* GetCollisionHandler() { return m_CollisionHandler.get(); };
    const CollisionHandler* GetCollisionHandler() const { return m_CollisionHandler.get(); };
    const TerrainHandler* GetTerrainHandler() const { return m_TerrainHandler.get(); };
    const VistaHandler* GetVistaHandler() const { return m_VistaHandler.get(); };
    const PlayerHandler* GetPlayerHandler() const { return m_PlayerHandler.get(); };
    const InventoryHandler* GetInventoryHandler() const { return m_InventoryHandler.get(); };
    const SpawningHandler* GetSpawningHandler() const { return m_SpawningHandler.get(); };

    void HandleEvent(EngineEvent _event);
    void HandleCommandLineArgs(const CommandLineArgs& _args);

    void AddWorldEvent(WorldEvent _event);

    void DebugDraw(UIDrawInterface& _interface) const;

    const Planet* GetPlanet() const { return m_Planet.has_value() ? &m_Planet.value() : nullptr; };
    const EnvironmentState& GetEnvironmentState() const { return m_EnvironmentState; };
    const WorldZoneRegion& GetWorldBounds() const { return m_WorldBounds; };
    f32 GetTimeScale() const { return m_TimeScale; };
    f32 GetGravityStrength() const { return m_GravityStrength; };

    bool RequestZone(glm::ivec3 _position);

private:
    const WorldObject* FindWorldObject(const WorldObjectRef& _objectRef) const;

    void TransferEntitiesBetweenZones();
    void UpdateActiveZones();
    void SendWorldEvents();
    void VerifyWorld() const;

    bool TryLoadZone(glm::ivec3 _position);

    bool IsZoneLoading(glm::ivec3 _coords) const;
    bool IsZoneLoaded(glm::ivec3 _coords) const;


    template<typename T>
    void TransferComponent(WorldObject& _sourceObject, WorldZone& _destinationZone, WorldObject& _destinationObject)
    {
        T* component = ComponentAccess::GetComponent<T>(_sourceObject);

        if (component != nullptr)
        {
            _destinationZone.GetComponents<T>().push_back(*component);
            _destinationObject.GetComponentRef<T>() = _destinationZone.GetComponents<T>().size() - 1;
        }
    }

private:
    std::vector<WorldZone> m_ActiveZones;

    static constexpr u32 CONCURRENT_WORLD_ZONE_LOADERS_COUNT = 9;
    using WorldZoneLoaders = DynamicLoaderCollection<WorldZone, glm::ivec3, CONCURRENT_WORLD_ZONE_LOADERS_COUNT, World*, glm::ivec3, glm::vec3>;
    WorldZoneLoaders m_ZoneLoaders;

    std::string m_Name;
    WorldZoneRegion m_WorldBounds;

    WorldObjectDirectory m_Directory;

    std::shared_ptr<CollisionHandler> m_CollisionHandler;
    std::shared_ptr<PlayerHandler> m_PlayerHandler;
    std::shared_ptr<SpawningHandler> m_SpawningHandler;
    std::shared_ptr<TerrainHandler> m_TerrainHandler;
    std::shared_ptr<VistaHandler> m_VistaHandler;
    std::shared_ptr<InventoryHandler> m_InventoryHandler;
    std::shared_ptr<ParticleSystemHandler> m_ParticleSystemHandler;
    
    EventHandler<WorldEvent> m_WorldEventHandler;

    std::optional<Planet> m_Planet;
    EnvironmentState m_EnvironmentState;

    f32 m_TimeScale = 1.f;
    f32 m_GravityStrength = 20.f;

    bool m_FreezeZones = false;
};
