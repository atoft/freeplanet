//
// Created by alastair on 26/08/17.
//

#include "World.h"

#include <memory>

#include <src/world/FreelookCameraComponent.h>
#include <src/world/collision/CollisionHandler.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/terrain/TerrainHandler.h>
#include <src/world/vista/VistaHandler.h>
#include <src/profiling/Profiler.h>
#include <src/tools/MathsHelpers.h>
#include <src/tools/PropRecipe.h>

World::World(std::string _worldName, std::optional<Planet> _planet)
{
    m_Name = _worldName;

    m_PlayerHandler = std::make_unique<PlayerHandler>(this);
    m_CollisionHandler = std::make_unique<CollisionHandler>(this);
    m_TerrainHandler = std::make_unique<TerrainHandler>(this);
    m_VistaHandler = std::make_unique<VistaHandler>(this);

    m_Planet = _planet;

    constexpr u32 INITIAL_RESERVED_ZONES = 27;
    m_ActiveZones.reserve(INITIAL_RESERVED_ZONES);

    u32 zonesRadius = 16;
    glm::ivec3 spawnZoneCoordinates = glm::ivec3(0);

    if (_planet.has_value())
    {
        const f32 worldSpaceRadius = _planet->m_Radius + _planet->m_AtmosphereHeight + _planet->m_WorldPadding;
        zonesRadius = worldSpaceRadius / TerrainConstants::WORLD_ZONE_SIZE;
        spawnZoneCoordinates = glm::ivec3(0, _planet->m_Radius / TerrainConstants::WORLD_ZONE_SIZE, 0);
    }

    // TODO Make sure zones outside these bounds are never loaded.
    m_WorldBounds.m_Min = glm::ivec3(-zonesRadius, -zonesRadius, -zonesRadius);
    m_WorldBounds.m_Max = glm::ivec3( zonesRadius,  zonesRadius,  zonesRadius);

    // Construct an initial zone so it's possible to start spawning right away.
    // Normally zones are built async by a DynamicLoader.
    m_ActiveZones.emplace_back(this, spawnZoneCoordinates, glm::vec3(TerrainConstants::WORLD_ZONE_SIZE));
}

const WorldObject* World::FindWorldObject(const WorldObjectRef& _objectRef) const
{
    for (const WorldZone& zone : m_ActiveZones)
    {
        if (zone.GetCoordinates() == _objectRef.m_ZoneCoordinates)
        {
            return zone.GetWorldObject(_objectRef.m_LocalRef);
        }
    }
    return nullptr;
}

WorldZone* World::FindZoneAtCoordinates(glm::ivec3 _zoneCoordinates)
{
    for (WorldZone& zone : m_ActiveZones)
    {
        if (zone.GetCoordinates() == _zoneCoordinates)
        {
            return &zone;
        }
    }
    return nullptr;
}

const WorldZone* World::FindZoneAtCoordinates(glm::ivec3 _zoneCoordinates) const
{
    for (const WorldZone& zone : m_ActiveZones)
    {
        if (zone.GetCoordinates() == _zoneCoordinates)
        {
            return &zone;
        }
    }
    return nullptr;
}

void World::RegisterLocalPlayer(u32 _playerIndex)
{
    m_PlayerHandler->RegisterLocalPlayer(_playerIndex);
}

void World::SpawnPlayerInWorldZone(glm::ivec3 _zoneCoordinates)
{
    WorldZone* targetZone = FindZoneAtCoordinates(_zoneCoordinates);
    if (targetZone == nullptr)
    {
        // TODO If the zone is nullptr, activate the zone and its surroundings

        return;
    }

    WorldObjectID targetID = targetZone->ConstructPlayerInZone("Player");

    Player player;
    player.AttachWorldObject(targetID);

    m_VistaHandler->OnLocalPlayerWorldZoneChanged(_zoneCoordinates);

    m_ActivePlayers.push_back(player);
}

void World::SpawnPropInWorldZone(const WorldPosition& _worldPosition, const PropRecipe& _propRecipe)
{
    WorldZone* targetZone = FindZoneAtCoordinates(_worldPosition.m_ZoneCoordinates);
    if (targetZone == nullptr)
    {
        // TODO If the zone is nullptr, activate the zone and its surroundings

        return;
    }

    targetZone->ConstructPropInZone(_worldPosition.m_LocalPosition, _propRecipe);

}

void World::Update(TimeMS _delta)
{
    ProfileCurrentFunction();

    const f32 deltaWithTimeScale = _delta * m_TimeScale;

    if (m_ActiveZones.empty())
    {
        LogWarning("No active zones in " + m_Name);
    }

    for (WorldZone& zone : m_ActiveZones)
    {
        zone.Update(deltaWithTimeScale);
    }

    m_PlayerHandler->Update();
    m_CollisionHandler->Update(deltaWithTimeScale);
    m_TerrainHandler->Update(deltaWithTimeScale);
    m_VistaHandler->Update(deltaWithTimeScale);

    m_EnvironmentState.Update(deltaWithTimeScale);

    SendWorldEvents();

    if (!m_FreezeZones)
    {
        TransferEntitiesBetweenZones();
        UpdateActiveZones();
    }
}

void World::TransferEntitiesBetweenZones()
{
    struct PendingZoneTransfer
    {
        bool m_ShouldDelete = false;

        glm::ivec3 m_SourceZone;
        glm::ivec3 m_DestinationZone;

        WorldObject* m_Object;

        PendingZoneTransfer()
        {};

        PendingZoneTransfer(bool _shouldDelete,
                            glm::ivec3 _sourceZone,
                            glm::ivec3 _destinationZone)
                : m_ShouldDelete(_shouldDelete),
                  m_SourceZone(_sourceZone),
                  m_DestinationZone(_destinationZone)
        {};
    };
    std::vector<PendingZoneTransfer> worldObjectsToTransfer;

    for (WorldZone& zone : m_ActiveZones)
    {
        for (WorldObject& worldObject : zone.GetWorldObjects())
        {
            const glm::ivec3 relativeCoords = zone.ComputeRelativeCoordinates(worldObject.GetPosition());
            if (relativeCoords != glm::ivec3(0))
            {
                static int i = 0;

                glm::ivec3 destinationCoords = zone.GetCoordinates() + relativeCoords;

                WorldZone* destinationZone = FindZoneAtCoordinates(destinationCoords);

                PendingZoneTransfer zoneTransfer;
                zoneTransfer.m_SourceZone = zone.GetCoordinates();
                zoneTransfer.m_DestinationZone = destinationCoords;
                zoneTransfer.m_ShouldDelete = destinationZone == nullptr && !IsControlledByLocalPlayer(worldObject.GetWorldObjectID());
                zoneTransfer.m_Object = &worldObject;
                worldObjectsToTransfer.push_back(zoneTransfer);

                LogMessage(std::to_string(i++) + " The object " + worldObject.GetName()
                           + " is out of bounds. Moving from " + glm::to_string(zone.GetCoordinates()) + " to "
                           + glm::to_string(destinationCoords));

            }
        }
    }

    for (const PendingZoneTransfer& pendingTransfer : worldObjectsToTransfer)
    {
        // This requires some thought about WorldObjects and Components that hold on to resources
        // such as RenderComponents, texture handles etc.
        // We don't want those resources to be released and reacquired.

        WorldZone* sourceZone = FindZoneAtCoordinates(pendingTransfer.m_SourceZone);

        if (!pendingTransfer.m_ShouldDelete)
        {
            WorldZone* destinationZone = FindZoneAtCoordinates(pendingTransfer.m_DestinationZone);

            if (destinationZone == nullptr)
            {
                TryLoadZone(pendingTransfer.m_DestinationZone);
                continue;
            }

            assert(pendingTransfer.m_Object != nullptr);

            destinationZone->GetWorldObjects().push_back(*pendingTransfer.m_Object);
            WorldObject& destinationObject = destinationZone->GetWorldObjects().back();
            destinationObject.SetName(destinationObject.GetName());

            BipedComponent* bipedComponent = pendingTransfer.m_Object->GetBipedComponent();
            if (bipedComponent != nullptr)
            {
                destinationZone->GetBipedComponents().push_back(*bipedComponent);
                destinationObject.GetComponentRef<BipedComponent>() = destinationZone->GetBipedComponents().size() - 1;
            }

            ColliderComponent* colliderComponent = pendingTransfer.m_Object->GetColliderComponent();
            if (colliderComponent != nullptr)
            {
                destinationZone->GetColliderComponents().push_back(*colliderComponent);
                destinationObject.GetComponentRef<ColliderComponent>() =
                        destinationZone->GetColliderComponents().size() - 1;
            }

            FreelookCameraComponent* cameraComponent = pendingTransfer.m_Object->GetFreelookCameraComponent();
            if (cameraComponent != nullptr)
            {
                destinationZone->GetCameraComponents().push_back(*cameraComponent);
                destinationObject.GetComponentRef<FreelookCameraComponent>() =
                        destinationZone->GetCameraComponents().size() - 1;
            }

            RenderComponent* renderComponent = pendingTransfer.m_Object->GetRenderComponent();
            if (renderComponent != nullptr)
            {
                destinationZone->GetRenderComponents().push_back(*renderComponent);
                destinationObject.GetComponentRef<RenderComponent>() =
                        destinationZone->GetRenderComponents().size() - 1;
            }

            // TODO Other components

            glm::vec3 positionOffset =
                    static_cast<glm::vec3>(destinationZone->GetCoordinates() - sourceZone->GetCoordinates()) *
                    TerrainConstants::WORLD_ZONE_SIZE;
            destinationObject.SetPosition(destinationObject.GetPosition() - positionOffset);
            destinationObject.GetRef().m_ZoneCoordinates = pendingTransfer.m_DestinationZone;
            destinationObject.GetRef().m_LocalRef = destinationZone->GetWorldObjects().size() - 1;

            sourceZone->TransferWorldObjectOutOfZone(pendingTransfer.m_Object->GetWorldObjectID());

            m_Directory.OnWorldObjectTransferred(destinationObject.GetWorldObjectID(), destinationObject.GetRef());

            if (IsControlledByLocalPlayer(destinationObject.GetWorldObjectID()))
            {
                m_VistaHandler->OnLocalPlayerWorldZoneChanged(pendingTransfer.m_DestinationZone);
            }

            assert(destinationZone->ComputeRelativeCoordinates(destinationObject.GetPosition()) == glm::ivec3(0));
        }
        else
        {
            sourceZone->DestroyWorldObject(pendingTransfer.m_Object->GetWorldObjectID());
            m_Directory.UnregisterWorldObject(pendingTransfer.m_Object->GetWorldObjectID());
        }
    }
}

void World::UpdateActiveZones()
{
    if (!m_ActivePlayers.empty())
    {
        std::vector<glm::ivec3> zonesToRemove;

        // @Performance Could get better cache coherence if the list of coordinates was separate from list of zones in general.
        for (const WorldZone& zone : m_ActiveZones)
        {
            zonesToRemove.push_back(zone.GetCoordinates());
        }

        // Remove zones which are far from any players.
        for (const Player& player : m_ActivePlayers)
        {
            const WorldObjectRef objectRef = m_Directory.GetWorldObjectLocation(player.GetControlledWorldObjectID());
            if (!objectRef.IsValid())
            {
                continue;
            }

            const glm::ivec3 playerZone = objectRef.m_ZoneCoordinates;

            for (u32 zoneIdx = zonesToRemove.size(); zoneIdx > 0; --zoneIdx)
            {
                const glm::ivec3 zone = zonesToRemove[zoneIdx - 1];
                const glm::ivec3 coordDistance = glm::abs(zone - playerZone);

                bool isFarFromPlayer = coordDistance.x > TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.x
                                    || coordDistance.y > TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.y
                                    || coordDistance.z > TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.z;

                if (!isFarFromPlayer)
                {
                    zonesToRemove.erase(zonesToRemove.begin() + (zoneIdx - 1));
                }
            }
        }

        m_ActiveZones.erase(std::remove_if(m_ActiveZones.begin(), m_ActiveZones.end(),
                                           [zonesToRemove](WorldZone &zone)
                                           {
                                               const bool shouldRemove = std::find(zonesToRemove.begin(),
                                                                                   zonesToRemove.end(),
                                                                                   zone.GetCoordinates()) != zonesToRemove.end();

                                               if (shouldRemove)
                                               {
                                                   LogMessage("Removing " + glm::to_string(zone.GetCoordinates()));
                                                   zone.OnRemovedFromWorld();
                                               }
                                               return shouldRemove;
                                           }), m_ActiveZones.end());

        // Add zones which are close to any players.
        for (const Player& player : m_ActivePlayers)
        {
            const WorldObjectRef objectRef = m_Directory.GetWorldObjectLocation(player.GetControlledWorldObjectID());
            if (!objectRef.IsValid())
            {
                continue;
            }

            const glm::ivec3 playerZone = objectRef.m_ZoneCoordinates;

            for (s32 z = -TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.z; z <= TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.z; ++z)
            {
                for (s32 y = -TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.y; y <= TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.y; ++y)
                {
                    for (s32 x = -TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.x; x <= TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.x; ++x)
                    {
                        const glm::ivec3 coords = playerZone + glm::ivec3(x, y, z);
                        if (!IsZoneLoaded(coords) && !IsZoneLoading(coords))
                        {
                            TryLoadZone(coords);
                        }
                    }
                }
            }
        }
    }

    // Check for zones that have finished loading
    m_ZoneLoaders.TransferLoadedContent(m_ActiveZones);
}

void World::SendWorldEvents()
{
    std::vector<WorldEvent> events = m_WorldEventHandler.PopEvents();

    for (const WorldEvent& event : events)
    {
        m_TerrainHandler->HandleWorldEvent(event);
    }
}

bool World::TryLoadZone(glm::ivec3 _position)
{
    const bool wasLoadPossible = m_ZoneLoaders.RequestLoad(_position, this, _position, glm::vec3(TerrainConstants::WORLD_ZONE_SIZE));

    return wasLoadPossible;
}

bool World::IsZoneLoading(glm::ivec3 _coords) const
{
    return m_ZoneLoaders.IsLoadingOrLoaded(_coords);
}

bool World::IsZoneLoaded(glm::ivec3 _coords) const
{
    return FindZoneAtCoordinates(_coords) != nullptr;
}

std::string World::GetName()
{
    return m_Name;
}

void World::OnButtonInput(InputType _inputType)
{
    for (WorldZone& zone : m_ActiveZones)
    {
        zone.OnButtonInput(_inputType);
    }
}

void World::OnMouseInput(f32 _mouseX, f32 _mouseY)
{
    for (WorldZone& zone : m_ActiveZones)
    {
        zone.OnMouseInput(_mouseX, _mouseY);
    }
}

const WorldObject* World::GetWorldObject(WorldObjectID _objectID) const
{
    const WorldObjectRef objectRef = m_Directory.GetWorldObjectLocation(_objectID);
    if (!objectRef.IsValid())
    {
        return nullptr;
    }

    return FindWorldObject(objectRef);
}

WorldObject* World::GetWorldObject(WorldObjectID _objectID)
{
    return const_cast<WorldObject*>(const_cast<const World*>(this)->GetWorldObject(_objectID));
}

void World::DestroyWorldObject(WorldObjectID _objectID)
{
    for (WorldZone& zone : m_ActiveZones)
    {
        if (zone.DestroyWorldObject(_objectID))
        {
            m_Directory.UnregisterWorldObject(_objectID);
            return;
        }
    }

    LogError("Couldn't find the object to destroy (ID" + std::to_string(_objectID) + ")");
    assert(false);
}

bool World::IsPlayerInZone(glm::ivec3 _coords) const
{
    for (const Player& player : m_ActivePlayers)
    {
        WorldObjectID playerControlledObjectID = player.GetControlledWorldObjectID();

        if (playerControlledObjectID != WORLDOBJECTID_INVALID)
        {
            return m_Directory.GetWorldObjectLocation(playerControlledObjectID).m_ZoneCoordinates == _coords;
        }
    }

    return false;
}

std::vector<WorldObjectID> World::GetLocalPlayers() const
{
    std::vector<WorldObjectID> result;

    for (const Player& player : m_ActivePlayers)
    {
        if (player.m_IsLocal)
        {
            result.push_back(player.GetControlledWorldObjectID());
        }
    }
    return result;
}

bool World::IsControlledByLocalPlayer(WorldObjectID _id) const
{
    for (const Player& player : m_ActivePlayers)
    {
        if (player.m_IsLocal && player.GetControlledWorldObjectID() == _id)
        {
            return true;
        }
    }

    return false;
}

void World::HandleEvent(EngineEvent _event)
{
    switch (_event.GetType())
    {
    case EngineEvent::Type::WorldRequestDestroyWorldObject:
    {
        const WorldObjectID id = _event.GetIntData();
        static_assert(sizeof(_event.GetIntData()) == sizeof(id));

        if (id == WORLDOBJECTID_INVALID)
        {
            LogError("Tried to destroy a WorldObject using the Invalid ID.");
            return;
        }

        LogMessage("Attempting to destroy WorldObject ID" + std::to_string(id));

        DestroyWorldObject(id);

        break;
    }
    case EngineEvent::Type::WorldSetPlanetaryRotationSpeedScale:
    {
        const f32 scale = _event.GetFloatData();
        m_EnvironmentState.SetPlanetaryRotationSpeedScale(scale);

        break;
    }
    case EngineEvent::Type::WorldSetTimeScale:
    {
        const f32 timeScale = _event.GetFloatData();

        if (timeScale >= 0.f && !MathsHelpers::IsNaN(timeScale))
        {
            m_TimeScale = timeScale;
        }

        break;
    }
    case EngineEvent::Type::WorldSetCollision:
    {
        m_CollisionHandler->SetShouldResolveCollisions(_event.GetIntData() == 1);

        break;
    }
    case EngineEvent::Type::WorldSetGravity:
    {
        const f32 gravity = _event.GetFloatData();

        if (gravity >= 0.f && !MathsHelpers::IsNaN(gravity))
        {
            m_GravityStrength = gravity;
        }

        break;
    }
    case EngineEvent::Type::WorldFreezeZones:
    {
        m_FreezeZones = _event.GetIntData() == 1;

        break;
    }
    case EngineEvent::Type::TerrainSetNormalGenerationMethod:
    {
        const NormalGenerationMethod method = static_cast<NormalGenerationMethod>(_event.GetIntData());

        if(method < NormalGenerationMethod::Count)
        {
            m_TerrainHandler->SetNormalGenerationMethod(method);
        }

        break;
    }
    default:
        break;
    }
}

void World::HandleCommandLineArgs(const CommandLineArgs& _args)
{
    if (_args.m_Noclip)
    {
        m_GravityStrength = 0.f;
        m_CollisionHandler->SetShouldResolveCollisions(false);
    }
}

void World::AddWorldEvent(WorldEvent _event)
{
    m_WorldEventHandler.PushEvent(_event);
}

void World::DebugDraw(UIDrawInterface& _interface) const
{
    _interface.DrawRectangle(glm::vec2(1500, 0), glm::ivec2(1920, 900), Color(0, 0, 0, 0.5f));

    std::string activeZones = "Active Zones:\n";

    for (const WorldZone& zone : m_ActiveZones)
    {
        activeZones += glm::to_string(zone.GetCoordinates());
        activeZones += "\n";
    }

    _interface.DrawString(glm::vec2(1520, 20), activeZones, 24.f);

    std::string loadingZones = "Loading Zones:\n";

    for (const glm::ivec3 zoneCoordinates : m_ZoneLoaders.GetLoadingOrLoadedIdentifiers())
    {
        loadingZones += glm::to_string(zoneCoordinates);
        loadingZones += "\n";
    }

    _interface.DrawString(glm::vec2(1700, 20), loadingZones, 24.f, Color(0, 1.f, 1.f, 1.f));
}
