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

#include <src/world/World.h>

#include <memory>

#include <src/engine/inspection/InspectionHelpers.h>
#include <src/world/BipedHandler.h>
#include <src/world/CameraHandler.h>
#include <src/world/SpawningHandler.h>
#include <src/world/collision/CollisionHandler.h>
#include <src/world/inventory/InventoryHandler.h>
#include <src/world/particles/ParticleSystemHandler.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/terrain/TerrainHandler.h>
#include <src/world/WorldSave.h>
#include <src/world/WorldZoneSave.h>
#include <src/world/vista/VistaHandler.h>
#include <src/profiling/Profiler.h>
#include <src/tools/MathsHelpers.h>
#include <src/tools/PropRecipe.h>
#include <src/world/planet/PlanetGeneration.h>

World::World()
{
    m_BipedHandler = std::make_shared<BipedHandler>(this);
    m_PlayerHandler = std::make_shared<PlayerHandler>(this);
    m_CollisionHandler = std::make_shared<CollisionHandler>(this);
    m_SpawningHandler = std::make_shared<SpawningHandler>(this);
    m_TerrainHandler = std::make_shared<TerrainHandler>(this);
    m_VistaHandler = std::make_shared<VistaHandler>(this);
    m_InventoryHandler = std::make_shared<InventoryHandler>(this);
    m_ParticleSystemHandler = std::make_shared<ParticleSystemHandler>(this);
    m_CameraHandler = std::make_shared<CameraHandler>(this);

    constexpr u32 INITIAL_RESERVED_ZONES = 27;
    m_ActiveZones.reserve(INITIAL_RESERVED_ZONES);
}

bool World::InitializeNew(const std::string& _name, std::optional<u32> _planetSeed)
{
    const std::string worldPath = "saved/" + _name;

    if (std::filesystem::exists(worldPath))
    {
        LogError("The desired world name already exists.");
        return false;
    }

    // Make a directory for savegames.
    std::filesystem::create_directory(worldPath);

    m_Name = _name;

    if (_planetSeed.has_value())
    {
        m_Planet = PlanetGeneration::GenerateFromSeed(*_planetSeed);
    }

    return true;
}

bool World::LoadFromFile(const std::string& _name)
{
    const std::string worldPath = "saved/" + _name + "/World.frpl";

    const InspectionHelpers::LoadFromBinaryFileResult<WorldSave> result = InspectionHelpers::LoadFromBinaryFile<WorldSave>(worldPath);

    if (result.m_Result == FromBinaryInspectionResult::Success)
    {
        const WorldSave save = *result.m_Value;

        if (save.m_HasPlanet)
        {
            m_Planet = PlanetGeneration::GenerateFromSeed(result.m_Value->m_Seed);
        }

        // TODO Use name from data m_Name = result.m_Value->m_Name;
        m_Name = _name;
        return true;
    }
    else if (result.m_Result == FromBinaryInspectionResult::FileIOError)
    {
        LogError("Failed to open world file.");
        return false;
    }

    LogError("World save is incomplete.");
    return false;
}

std::vector<std::string> World::GetSavedWorldNames()
{
    std::vector<std::string> names;

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("saved"))
    {
        if (!entry.is_directory())
        {
            continue;
        }

        const bool hasWorldFile = std::filesystem::exists(entry.path().string() + "/World.frpl");

        // Could pre-read some data from the World file here to show a preview.

        if (hasWorldFile)
        {
            names.push_back(entry.path().filename().string());
        }
    }

    return names;
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
    LogError("A WorldObject " + glm::to_string(_objectRef.m_ZoneCoordinates) + ", " + std::to_string(_objectRef.m_LocalRef) + " couldn't be found.");
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

    m_InventoryHandler->RegisterLocalPlayer(_playerIndex);
}

// TODO remove this entirely and send test world props through a similar flow to procedural objects.
void World::SpawnPropInWorldZone(const WorldPosition& _worldPosition, const PropRecipe& _propRecipe)
{
    WorldZone* targetZone = FindZoneAtCoordinates(_worldPosition.m_ZoneCoordinates);
    if (targetZone == nullptr)
    {
        return;
    }

    WorldObject& newObject = ConstructWorldObject(*targetZone, _propRecipe.m_Name);
    newObject.SetInitialPosition(_worldPosition.m_LocalPosition);

    // Do we want this as a method on WorldObject, or do we not want euler angles to be generally used?
    newObject.Rotate(glm::vec3(1,0,0), _propRecipe.m_PitchYawRoll.x);
    newObject.Rotate(glm::vec3(0,1,0), _propRecipe.m_PitchYawRoll.y);
    newObject.Rotate(glm::vec3(0,0,1), _propRecipe.m_PitchYawRoll.z);

    // TODO Scale must be applied after rotation otherwise we will get a skew from non-uniform scales.
    // Don't want to ban non-uniform scale as it's useful for testing so we should make the WorldObject API
    // able to handle it correctly.
    newObject.SetScale(_propRecipe.m_Scale);

    ColliderComponent& collider = targetZone->AddComponent<ColliderComponent>(newObject, CollisionPrimitiveType::OBB, MovementType::Fixed);
    collider.m_Bounds = AABB(_propRecipe.m_Scale / 2.f);

    targetZone->AddComponent<RenderComponent>(newObject, AssetHandle<StaticMesh>(_propRecipe.m_MeshID),
                                    AssetHandle<ShaderProgram>(_propRecipe.m_ShaderID),
                                    AssetHandle<Texture>(_propRecipe.m_TextureID),
                                    MeshType::Normal);
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

    m_BipedHandler->Update(deltaWithTimeScale);
    m_PlayerHandler->Update();
    m_CollisionHandler->Update(deltaWithTimeScale);
    m_SpawningHandler->Update();
    m_TerrainHandler->Update(deltaWithTimeScale);
    m_VistaHandler->Update(deltaWithTimeScale);
    m_ParticleSystemHandler->Update(deltaWithTimeScale);
    m_CameraHandler->Update(deltaWithTimeScale);

    m_EnvironmentState.Update(deltaWithTimeScale);

    SendWorldEvents();

    if (!m_FreezeZones)
    {
        TransferEntitiesBetweenZones();
        UpdateActiveZones();
        VerifyWorld();
    }
}

void World::TransferEntitiesBetweenZones()
{
    struct PendingZoneTransfer
    {
        bool m_ShouldDelete = false;

        glm::ivec3 m_SourceZone = glm::ivec3();
        glm::ivec3 m_DestinationZone = glm::ivec3();

        WorldObjectID m_ObjectID = WORLDOBJECTID_INVALID;

        PendingZoneTransfer() = default;
    };

    std::vector<PendingZoneTransfer> worldObjectsToTransfer;

    for (WorldZone& zone : m_ActiveZones)
    {
        for (WorldObject& worldObject : zone.GetWorldObjects())
        {
            const WorldPosition worldPosition = worldObject.GetWorldPosition();
            
            if (!worldPosition.IsInsideZone())
            {
                static int i = 0;

                glm::ivec3 destinationCoords = worldPosition.GetInsideZone().m_ZoneCoordinates;

                WorldZone* destinationZone = FindZoneAtCoordinates(destinationCoords);

                PendingZoneTransfer zoneTransfer;
                zoneTransfer.m_SourceZone = zone.GetCoordinates();
                zoneTransfer.m_DestinationZone = destinationCoords;
                zoneTransfer.m_ShouldDelete = destinationZone == nullptr && !m_PlayerHandler->IsControlledByLocalPlayer(worldObject.GetWorldObjectID());
                zoneTransfer.m_ObjectID = worldObject.GetWorldObjectID();
                worldObjectsToTransfer.push_back(zoneTransfer);

                LogMessage(std::to_string(i++) + " The object " + worldObject.GetName()
                           + " ID"
                           + std::to_string(worldObject.GetWorldObjectID())
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
        assert(sourceZone != nullptr);
        
        // Get it by ID in case it was moved in memory before we got to process the transfer. 
        WorldObject* sourceObject = GetWorldObject(pendingTransfer.m_ObjectID);
        assert(sourceObject != nullptr);
        
        if (!pendingTransfer.m_ShouldDelete)
        {
            WorldZone* destinationZone = FindZoneAtCoordinates(pendingTransfer.m_DestinationZone);

            if (destinationZone == nullptr)
            {
                TryLoadZone(pendingTransfer.m_DestinationZone);
                continue;
            }
            
            destinationZone->GetWorldObjects().push_back(*sourceObject);
            WorldObject& destinationObject = destinationZone->GetWorldObjects().back();
            destinationObject.SetName(destinationObject.GetName());

            TransferComponent<BipedComponent>(*sourceObject, *destinationZone, destinationObject);
            TransferComponent<ColliderComponent>(*sourceObject, *destinationZone, destinationObject);
            TransferComponent<FreelookCameraComponent>(*sourceObject, *destinationZone, destinationObject);
            TransferComponent<RenderComponent>(*sourceObject, *destinationZone, destinationObject);
            TransferComponent<LightComponent>(*sourceObject, *destinationZone, destinationObject);
            TransferComponent<ParticleSystemComponent>(*sourceObject, *destinationZone, destinationObject);
            static_assert(ComponentConstants::ComponentCount == 6);

            glm::vec3 positionOffset =
                    static_cast<glm::vec3>(destinationZone->GetCoordinates() - sourceZone->GetCoordinates()) *
                    TerrainConstants::WORLD_ZONE_SIZE;
            destinationObject.SetPosition(destinationObject.GetPosition() - positionOffset);
            destinationObject.GetRef().m_ZoneCoordinates = pendingTransfer.m_DestinationZone;
            destinationObject.GetRef().m_LocalRef = destinationZone->GetWorldObjects().size() - 1;

            assert(destinationObject.GetWorldPosition().IsInsideZone());
            assert(destinationObject.GetWorldPosition().m_ZoneCoordinates == destinationZone->GetCoordinates());
            
            sourceZone->TransferWorldObjectOutOfZone(sourceObject->GetWorldObjectID());

            // No longer safe to access.
            sourceObject = nullptr;
            
            m_Directory.OnWorldObjectTransferred(destinationObject.GetWorldObjectID(), destinationObject.GetRef());

            if (m_PlayerHandler->IsControlledByLocalPlayer(destinationObject.GetWorldObjectID()))
            {
                m_VistaHandler->OnLocalPlayerWorldZoneChanged(pendingTransfer.m_DestinationZone);
            }
        }
        else
        {
            sourceZone->DestroyWorldObject(pendingTransfer.m_ObjectID);

            // No longer safe to access.
            sourceObject = nullptr;
            
            m_Directory.UnregisterWorldObject(pendingTransfer.m_ObjectID);
        }
    }
}

void World::UpdateActiveZones()
{
    if (!m_PlayerHandler->GetLocalPlayers().empty())
    {
        std::vector<glm::ivec3> zonesToRemove;

        // @Performance Could get better cache coherence if the list of coordinates was separate from list of zones in general.
        for (const WorldZone& zone : m_ActiveZones)
        {
            zonesToRemove.push_back(zone.GetCoordinates());
        }

        // Remove zones which are far from any players.
        for (const WorldObjectID& playerControlledObjectID : m_PlayerHandler->GetLocalPlayers())
        {
            const glm::ivec3 playerZone = LocateWorldObject(playerControlledObjectID);

            for (u32 zoneIdx = zonesToRemove.size(); zoneIdx > 0; --zoneIdx)
            {
                const glm::ivec3 zone = zonesToRemove[zoneIdx - 1];
                const glm::ivec3 coordDistance = glm::abs(zone - playerZone);

                const bool isFarFromPlayer = coordDistance.x > TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.x
                                    || coordDistance.y > TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.y
                                    || coordDistance.z > TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.z;

                if (!isFarFromPlayer)
                {
                    zonesToRemove.erase(zonesToRemove.begin() + (zoneIdx - 1));
                }
            }
        }

        // TODO replace this in refactor of WorldZones.
        for (WorldZone& zone : m_ActiveZones)
        {
            const bool shouldRemove = std::find(zonesToRemove.begin(),
                                                zonesToRemove.end(),
                                                zone.GetCoordinates()) != zonesToRemove.end();

            if (shouldRemove)
            {
                LogMessage("Removing " + glm::to_string(zone.GetCoordinates()));

                SaveZoneToFile(zone);
                
                zone.OnRemovedFromWorld();                
            }
        }

        STL::RemoveIf(m_ActiveZones, [&zonesToRemove](const WorldZone& zone)
                                     {
                                         const bool shouldRemove = std::find(zonesToRemove.begin(),
                                                                             zonesToRemove.end(),
                                                                             zone.GetCoordinates()) != zonesToRemove.end();
                                         return shouldRemove;
                                     });

        // Add zones which are close to any players.
        for (const WorldObjectID& playerControlledObjectID : m_PlayerHandler->GetLocalPlayers())
        {
            const glm::ivec3 playerZone = LocateWorldObject(playerControlledObjectID);

            for (s32 z = -TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.z; z <= TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.z; ++z)
            {
                for (s32 y = -TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.y; y <= TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.y; ++y)
                {
                    for (s32 x = -TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.x; x <= TerrainConstants::WORLD_ZONE_LOAD_DISTANCE.x; ++x)
                    {
                        const glm::ivec3 coords = playerZone + glm::ivec3(x, y, z);
                        RequestZone(coords);
                    }
                }
            }
        }
    }

    // Check for zones that have finished loading
    m_ZoneLoaders.TransferLoadedContent(m_ActiveZones);

    for (WorldZone& zone : m_ActiveZones)
    {
        if (!zone.m_HasLoadedFromSave && zone.IsProceduralSpawningDone())
        {
            LoadZoneFromFile(zone);
        }
    }
}

void World::LoadZoneFromFile(WorldZone& _zone)
{
    const glm::ivec3 zoneCoords = _zone.GetCoordinates();
    const std::string coords = std::to_string(zoneCoords.x) + "_" + std::to_string(zoneCoords.y) + "_" + std::to_string(zoneCoords.z);
    const std::string path = "saved/" + m_Name + "/" + coords + ".frpl";
    const InspectionHelpers::LoadFromBinaryFileResult<WorldZoneSave> loaded = InspectionHelpers::LoadFromBinaryFile<WorldZoneSave>(path);
    if (loaded.m_Result == FromBinaryInspectionResult::Success)
    {
        const WorldZoneSave& save = *loaded.m_Value;

        if (save.m_ZoneCoords == zoneCoords)
        {
            _zone.GetTerrainComponent().m_TerrainEdits = save.m_TerrainEdits;
            LogMessage("Loaded " + std::to_string(save.m_TerrainEdits.m_AdditiveElements.size() + save.m_TerrainEdits.m_AdditiveElements.size()) + " edits from save.");
            _zone.GetTerrainComponent().SetAllDirty();
        }
        else
        {
            LogError("Error attempting to load save for " + glm::to_string(zoneCoords) + " - file contains the wrong coordinates.");
        }
    }
    else if (loaded.m_Result == FromBinaryInspectionResult::ReadSyntaxError)
    {
        LogError("Error attempting to load save for " + glm::to_string(zoneCoords) + " - file was malformed.");
    }

    _zone.m_HasLoadedFromSave = true;
}

void World::SaveZoneToFile(WorldZone &_zone)
{
    WorldZoneSave save;
    save.m_ZoneCoords = _zone.GetCoordinates();
    save.m_TerrainEdits = _zone.GetTerrainComponent().m_TerrainEdits;

    const bool bIsZoneEdited = !save.m_TerrainEdits.IsEmpty();

    if (!bIsZoneEdited)
    {
        return;
    }

    const std::string coords = std::to_string(save.m_ZoneCoords.x) + "_" + std::to_string(save.m_ZoneCoords.y) + "_" + std::to_string(save.m_ZoneCoords.z);
    InspectionHelpers::SaveToBinaryFile(save, "saved/" + m_Name + "/" + coords + ".frpl");
}

void World::SaveWorldToFile()
{
    WorldSave save;
    save.m_Name = m_Name;

    save.m_HasPlanet = m_Planet.has_value();

    if (m_Planet.has_value())
    {
        save.m_Seed = m_Planet->m_TerrainSeed;
    }

    InspectionHelpers::SaveToBinaryFile(save, "saved/" + m_Name + "/World.frpl");
}

void World::SendWorldEvents()
{
    std::vector<WorldEvent> events = m_WorldEventHandler.PopEvents();

    for (const WorldEvent& event : events)
    {
        m_SpawningHandler->HandleWorldEvent(event);
        m_TerrainHandler->HandleWorldEvent(event);
    }
}

bool World::RequestZone(glm::ivec3 _position)
{
    if (IsZoneLoaded(_position) || IsZoneLoading(_position))
    {
        return true;
    }

    return TryLoadZone(_position);
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
    constexpr u32 localPlayerIdx = 0;

    const WorldObjectID controlledId = GetPlayerHandler()->GetControlledWorldObjectID(localPlayerIdx);

    WorldObject* controlledWorldObject = nullptr;
    
    if (controlledId != WORLDOBJECTID_INVALID)
    {
        controlledWorldObject = GetWorldObject(controlledId);
        assert(controlledWorldObject != nullptr);
    }

    m_BipedHandler->OnButtonInput(localPlayerIdx, controlledWorldObject,_inputType);
    m_InventoryHandler->OnButtonInput(localPlayerIdx, controlledWorldObject, _inputType);
}

void World::OnMouseInput(f32 _mouseX, f32 _mouseY)
{
    constexpr u32 localPlayerIdx = 0;
    
    const WorldObjectID controlledId = GetPlayerHandler()->GetControlledWorldObjectID(localPlayerIdx);

    WorldObject* controlledWorldObject = nullptr;
    
    if (controlledId != WORLDOBJECTID_INVALID)
    {
        controlledWorldObject = GetWorldObject(controlledId);
        assert(controlledWorldObject != nullptr);
    }

    m_CameraHandler->OnMouseInput(localPlayerIdx, controlledWorldObject, _mouseX, _mouseY);
}

const WorldObject* World::GetWorldObject(WorldObjectID _objectID) const
{
    const WorldObjectRef objectRef = m_Directory.GetWorldObjectLocation(_objectID);
    if (!objectRef.IsValid())
    {
        LogError("A worldObject ID" + std::to_string(_objectID) + " couldn't be found in the directory.");
        return nullptr;
    }

    return FindWorldObject(objectRef);
}

WorldObject* World::GetWorldObject(WorldObjectID _objectID)
{
    return const_cast<WorldObject*>(const_cast<const World*>(this)->GetWorldObject(_objectID));
}

glm::ivec3 World::LocateWorldObject(WorldObjectID _objectID) const
{
    return m_Directory.GetWorldObjectLocation(_objectID).m_ZoneCoordinates;
}

WorldObject& World::ConstructWorldObject(WorldZone& _zone, const std::string& _name)
{
    WorldObject& newObject = _zone.GetWorldObjects().emplace_back(this);

    // Create a unique reference for this WorldObject.
    newObject.GetRef().m_ZoneCoordinates = _zone.GetCoordinates();
    newObject.GetRef().m_LocalRef = _zone.GetWorldObjects().size() - 1;

    const WorldObjectID newID = m_Directory.RegisterWorldObject(newObject.GetRef());

    newObject.SetWorldObjectID(newID);
    newObject.SetName(_name);
    newObject.SetInitialPosition(glm::vec3(0.f));

    return newObject;
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

void World::OnPlayerSpawned(const WorldObject& _controlledObject)
{
    m_VistaHandler->OnLocalPlayerWorldZoneChanged(_controlledObject.GetWorldPosition().m_ZoneCoordinates);
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
    case EngineEvent::Type::OnQuit:
    {
        for (WorldZone& zone : m_ActiveZones)
        {
            SaveZoneToFile(zone);
        }
        SaveWorldToFile();
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

void World::VerifyWorld() const
{
    u32 totalWorldObjects = 0;

    for (const WorldZone& zone : m_ActiveZones)
    {
        totalWorldObjects += zone.GetWorldObjects().size();
    }

    if (totalWorldObjects != m_Directory.GetCount())
    {
        LogError("Directory is out of sync with the contents of the zones.");
        assert(false);
    }
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

    // TODO separate debug UI for this.
    m_SpawningHandler->DebugDraw(_interface);
}

