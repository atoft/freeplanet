
#include "WorldZone.h"

#include <glm/gtc/matrix_transform.hpp>

#include <src/engine/Engine.h>
#include <src/world/WorldObject.h>
#include <src/world/World.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/profiling/Profiler.h>

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
    m_LightComponents.reserve(INITIAL_COMPONENT_COUNT);
    m_ParticleSystemComponents.reserve(INITIAL_COMPONENT_COUNT);
    static_assert(ComponentConstants::ComponentCount == 6);

    // TODO either remove this requirement or replace dimensions with f32
    assert(_dimensions.x == _dimensions.y && _dimensions.y == _dimensions.z);

    const f32 chunksPerEdge = Engine::GetInstance().GetCommandLineArgs().m_TriangleDebug ? 1 : TerrainConstants::CHUNKS_PER_ZONE_EDGE;

    m_TerrainComponent = TerrainComponent(chunksPerEdge, _dimensions.x / chunksPerEdge, _position);
}

void WorldZone::OnRemovedFromWorld()
{
    // TODO this split of creation and destruction between world and zone is confusing and bug-prone, it should all
    // move up to the world and make the zones more POD-like.
    for (const WorldObject& object : m_WorldObjects)
    {
        m_OwnerWorld->DestroyWorldObject(object.GetWorldObjectID());
    }

    m_TerrainComponent.m_DynamicMesh.RequestDestruction();
}

template<typename Type>
bool DestroyComponentOfObject(std::vector<Type>& _components, WorldObjectID _objectID, bool _isBeingRemovedFromWorld)
{
    for (LocalComponentRef componentIdx = 0; componentIdx < _components.size(); ++componentIdx)
    {
        Type& component = _components[componentIdx];

        if (component.GetOwner() == _objectID)
        {
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
    DestroyComponentOfObject(m_LightComponents, _objectID, _removedFromWorld);
    DestroyComponentOfObject(m_ParticleSystemComponents, _objectID, _removedFromWorld);
    static_assert(ComponentConstants::ComponentCount == 6);

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

    for (WorldObject& object : m_WorldObjects)
    {
        object.Update(delta);
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

bool WorldZone::ContainsPlayer() const
{
    return m_OwnerWorld->GetPlayerHandler()->IsPlayerInZone(m_Coordinates);
}
