
#include "WorldZone.h"

#include <glm/gtc/matrix_transform.hpp>

#include <src/engine/Engine.h>
#include <src/world/WorldObject.h>
#include <src/world/World.h>
#include <src/world/terrain/elements/PerlinTerrainElement.h>
#include <src/world/terrain/elements/PlaneTerrainElement.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/profiling/Profiler.h>
#include <src/tools/PropRecipe.h>

template<typename Type>
bool DestroyComponentOfObject(std::vector<Type>& _components, WorldObjectID _objectID, bool _isBeingRemovedFromWorld);

WorldZone::WorldZone(World* _world, glm::ivec3 _position, glm::vec3 _dimensions)
{
    m_OwnerWorld = _world;
    m_Coordinates = _position;
    m_Dimensions = _dimensions;

    m_WorldObjects.reserve(INITIAL_WORLDOBJECT_COUNT);

    m_BipedComponents.reserve(INITIAL_COMPONENT_COUNT);
    m_ColliderComponents.reserve(INITIAL_COMPONENT_COUNT);
    m_CameraComponents.reserve(INITIAL_COMPONENT_COUNT);
    m_RenderComponents.reserve(INITIAL_COMPONENT_COUNT);

    // TODO either remove this requirement or replace dimensions with f32
    assert(_dimensions.x == _dimensions.y && _dimensions.y == _dimensions.z);

    const f32 chunksPerEdge = Engine::GetInstance().GetCommandLineArgs().m_TriangleDebug ? 1 : TerrainConstants::CHUNKS_PER_ZONE_EDGE;

    m_TerrainComponent = TerrainComponent(chunksPerEdge, _dimensions.x / chunksPerEdge, _position);
}

void WorldZone::OnRemovedFromWorld()
{
    for (const WorldObject& object : m_WorldObjects)
    {
        DestroyComponentOfObject(m_BipedComponents, object.GetWorldObjectID(), true);
        DestroyComponentOfObject(m_ColliderComponents, object.GetWorldObjectID(), true);
        DestroyComponentOfObject(m_CameraComponents, object.GetWorldObjectID(), true);
        DestroyComponentOfObject(m_RenderComponents, object.GetWorldObjectID(), true);
    }

    m_TerrainComponent.m_DynamicMesh.RequestDestruction();
}

WorldObjectID WorldZone::ConstructPlayerInZone(std::string _name)
{
    // TODO Make encapsulated methods for generic worldObject construction

    m_WorldObjects.emplace_back(m_OwnerWorld);
    WorldObject& newObject = m_WorldObjects.back();

    // Create a unique reference for this WorldObject
    newObject.GetRef().m_ZoneCoordinates = m_Coordinates;
    newObject.GetRef().m_LocalRef = m_WorldObjects.size() - 1;

    const WorldObjectID newID = m_OwnerWorld->RegisterWorldObject(newObject.GetRef());

    newObject.SetWorldObjectID(newID);
    newObject.SetName(_name);
    newObject.SetInitialPosition(glm::vec3(0, 2, 0));

    m_ColliderComponents.emplace_back(m_OwnerWorld, newID, CollisionPrimitiveType::OBB, MovementType::Movable);
    m_ColliderComponents.back().m_Bounds = glm::vec3(.5f, 1.f, .5f);
    m_ColliderComponents.back().m_KeepUpright = true;
    newObject.GetComponentRef<ColliderComponent>() = m_ColliderComponents.size() - 1;

    m_CameraComponents.emplace_back(m_OwnerWorld, newID);
    FreelookCameraComponent& cameraComponent = m_CameraComponents.back();
    cameraComponent.RequestMakeActive();
    newObject.GetComponentRef<FreelookCameraComponent>() = m_CameraComponents.size() - 1;


    ComponentRef cameraRef;
    cameraRef.m_ZoneCoordinates = m_Coordinates;
    cameraRef.m_LocalRef = newObject.GetComponentRef<FreelookCameraComponent>();

    m_BipedComponents.emplace_back(m_OwnerWorld, newID);
    newObject.GetComponentRef<BipedComponent>() = m_BipedComponents.size() - 1;

    return newID;
}

void WorldZone::ConstructPropInZone(glm::vec3 _localCoordinates, const PropRecipe& _propRecipe)
{
    m_WorldObjects.emplace_back(m_OwnerWorld);
    WorldObject& newObject = m_WorldObjects.back();

    // Create a unique reference for this WorldObject
    newObject.GetRef().m_ZoneCoordinates = m_Coordinates;
    newObject.GetRef().m_LocalRef = m_WorldObjects.size() - 1;

    const WorldObjectID newID = m_OwnerWorld->RegisterWorldObject(newObject.GetRef());

    newObject.SetWorldObjectID(newID);
    newObject.SetName(_propRecipe.m_Name);
    newObject.SetInitialPosition(_localCoordinates);

    // Do we want this as a method on WorldObject, or do we not want euler angles to be generally used?
    newObject.Rotate(glm::vec3(1,0,0), _propRecipe.m_PitchYawRoll.x);
    newObject.Rotate(glm::vec3(0,1,0), _propRecipe.m_PitchYawRoll.y);
    newObject.Rotate(glm::vec3(0,0,1), _propRecipe.m_PitchYawRoll.z);

    // TODO Scale must be applied after rotation otherwise we will get a skew from non-uniform scales.
    // Don't want to ban non-uniform scale as it's useful for testing so we should make the WorldObject API
    // able to handle it correctly.
    newObject.SetScale(_propRecipe.m_Scale);

    m_ColliderComponents.emplace_back(m_OwnerWorld, newID, CollisionPrimitiveType::OBB, MovementType::Fixed);
    newObject.GetComponentRef<ColliderComponent>() = m_ColliderComponents.size() - 1;

    m_ColliderComponents.back().m_Bounds = _propRecipe.m_Scale / 2.f;

    m_RenderComponents.emplace_back(AssetHandle<StaticMesh>(_propRecipe.m_MeshID),
                                    AssetHandle<ShaderProgram>(_propRecipe.m_ShaderID),
                                    AssetHandle<Texture>(_propRecipe.m_TextureID), m_OwnerWorld, newID);
    newObject.GetComponentRef<RenderComponent>() = m_RenderComponents.size() - 1;
}

template<typename Type>
bool DestroyComponentOfObject(std::vector<Type>& _components, WorldObjectID _objectID, bool _isBeingRemovedFromWorld)
{
    for (LocalComponentRef componentIdx = 0; componentIdx < _components.size(); ++componentIdx)
    {
        Type& component = _components[componentIdx];

        if (component.GetOwner() == _objectID)
        {
            if (_isBeingRemovedFromWorld)
            {
                _components[componentIdx].OnRemovedFromWorld();
            }

            _components[componentIdx] = _components.back();

            // Update Ref to the previous last component in WorldObject
            _components[componentIdx].GetOwnerObject()->template GetComponentRef<Type>() = componentIdx;

            _components.pop_back();

            return true;
        }
    }
    return false;
}

bool WorldZone::DestroyWorldObject(WorldObjectID _objectID)
{
    return DestroyWorldObject_Internal(_objectID, true);
}

bool WorldZone::TransferWorldObjectOutOfZone(WorldObjectID _objectID)
{
    return DestroyWorldObject_Internal(_objectID, false);
}

bool WorldZone::DestroyWorldObject_Internal(WorldObjectID _objectID, bool _removedFromWorld)
{
    // TODO queue all destructions to end of frame to avoid references becoming invalid

    WorldObject* swapObject = m_OwnerWorld->GetWorldObject(_objectID);
    if (swapObject == nullptr || swapObject->GetRef().m_ZoneCoordinates != m_Coordinates)
    {
        return false;
    }

    WorldObjectRef globalRef;
    globalRef.m_ZoneCoordinates = m_Coordinates;

    DestroyComponentOfObject(m_BipedComponents, _objectID, _removedFromWorld);
    DestroyComponentOfObject(m_ColliderComponents, _objectID, _removedFromWorld);
    DestroyComponentOfObject(m_CameraComponents, _objectID, _removedFromWorld);
    DestroyComponentOfObject(m_RenderComponents, _objectID, _removedFromWorld);


    WorldObjectRef ref = swapObject->GetRef();
    *swapObject = m_WorldObjects.back();
    m_OwnerWorld->OnWorldObjectTransferred(swapObject->GetWorldObjectID(), ref);
    m_WorldObjects.pop_back();

    swapObject->GetRef() = ref;
    return true;
}

const WorldObject* WorldZone::GetWorldObject(LocalWorldObjectRef _ref) const
{
    assert(_ref != REF_INVALID && _ref < m_WorldObjects.size());

    return &m_WorldObjects[_ref];
}

void WorldZone::Update(TimeMS delta)
{
    ProfileCurrentFunction();

    for (auto& object : m_WorldObjects)
    {
        object.Update(delta);
    }

    for (auto& component : m_BipedComponents)
    {
        component.Update(delta);
    }
    for (auto& component : m_ColliderComponents)
    {
        component.Update(delta);
    }
    for (auto& component : m_CameraComponents)
    {
        component.Update(delta);
    }
    for (auto& component : m_RenderComponents)
    {
        component.Update(delta);
    }
}

void WorldZone::OnButtonInput(InputType _inputType)
{
    // TODO Pass the inputs only to the WorldObject associated with the player.
    for (BipedComponent& component : m_BipedComponents)
    {
        component.OnButtonInput(_inputType);
    }
}

void WorldZone::OnMouseInput(f32 _mouseX, f32 _mouseY)
{
    for (BipedComponent& component : m_BipedComponents)
    {
        component.OnMouseInput(_mouseX, _mouseY);
    }
    for (FreelookCameraComponent& component : m_CameraComponents)
    {
        component.OnMouseInput(_mouseX, _mouseY);
    }
}

glm::mat4x4 WorldZone::GetGlobalTransform() const
{
    glm::vec3 globalPosition = (static_cast<glm::vec3>(m_Coordinates)) * m_Dimensions;

    return glm::translate(glm::mat4(1.f), globalPosition);
}

glm::mat4x4 WorldZone::GetRelativeTransform(glm::ivec3 _relativeZone) const
{
    glm::vec3 globalPosition = (static_cast<glm::vec3>(m_Coordinates - _relativeZone)) * m_Dimensions;

    return glm::translate(glm::mat4(1.f), globalPosition);
}

glm::mat4x4 WorldZone::GetTerrainModelTransform() const
{
    return glm::translate(glm::mat4(1.f), glm::vec3(-0.5f) * m_Dimensions);
}

glm::ivec3 WorldZone::ComputeRelativeCoordinates(glm::vec3 _zonePosition) const
{
    const glm::ivec3 relativeCoords = glm::ivec3(glm::round(_zonePosition / m_Dimensions) );

    return relativeCoords;
}

bool WorldZone::ContainsPlayer() const
{
    return m_OwnerWorld->IsPlayerInZone(m_Coordinates);
}
