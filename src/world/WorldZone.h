//
// Created by alastair on 06/05/18.
//

#pragma once

#include <vector>

#include <src/tools/globals.h>
#include <src/world/terrain/TerrainComponent.h>
#include <src/world/WorldObject.h>

#include <src/world/BipedComponent.h>
#include <src/world/FreelookCameraComponent.h>
#include <src/world/collision/ColliderComponent.h>
#include <src/graphics/RenderComponent.h>
#include <src/world/terrain/TerrainComponent.h>

class World;
struct PropRecipe;

class WorldZone
{
public:
    WorldZone(World* _world, glm::ivec3 _position, glm::vec3 _dimensions, u32 _terrainSeed, bool _initTerrain);

    void Update(TimeMS delta);

    WorldObjectID ConstructPlayerInZone(std::string _name);
    void ConstructPropInZone(glm::vec3 _localCoordinates, const PropRecipe& _propRecipe);

    const WorldObject* GetWorldObject(LocalWorldObjectRef _ref) const;
    bool DestroyWorldObject(WorldObjectID _objectID);
    bool TransferWorldObjectOutOfZone(WorldObjectID _objectID);

    inline const glm::ivec3 GetCoordinates() const { return m_Coordinates; };
    inline const glm::vec3 GetDimensions() const { return m_Dimensions; };

    glm::mat4x4 GetGlobalTransform() const;
    glm::mat4x4 GetRelativeTransform(glm::ivec3 _relativeZone) const;
    glm::mat4x4 GetTerrainModelTransform() const;

    void OnButtonInput(InputType _inputType);
    void OnMouseInput(f32 _mouseX, f32 _mouseY);
    void OnRemovedFromWorld();

    const BipedComponent* FindBipedComponent(LocalComponentRef _ref) const
    {
        if(_ref < m_BipedComponents.size())
        {
            return &m_BipedComponents[_ref];
        }
        return nullptr;
    }

    const ColliderComponent* FindColliderComponent(LocalComponentRef _ref) const
    {
        if(_ref < m_ColliderComponents.size())
        {
            return &m_ColliderComponents[_ref];
        }
        return nullptr;
    }

    const FreelookCameraComponent* FindCameraComponent(LocalComponentRef _ref) const
    {
        if(_ref < m_CameraComponents.size())
        {
            return &m_CameraComponents[_ref];
        }
        return nullptr;
    }

    const RenderComponent* FindRenderComponent(LocalComponentRef _ref) const
    {
        if(_ref < m_RenderComponents.size())
        {
            return &m_RenderComponents[_ref];
        }
        return nullptr;
    }

    BipedComponent* FindBipedComponent(LocalComponentRef _ref)
    {
        return const_cast<BipedComponent*>(static_cast<const WorldZone*>(this)->FindBipedComponent(_ref));
    }
    ColliderComponent* FindColliderComponent(LocalComponentRef _ref)
    {
        return const_cast<ColliderComponent*>(static_cast<const WorldZone*>(this)->FindColliderComponent(_ref));
    }
    FreelookCameraComponent* FindCameraComponent(LocalComponentRef _ref)
    {
        return const_cast<FreelookCameraComponent*>(static_cast<const WorldZone*>(this)->FindCameraComponent(_ref));
    }
    RenderComponent* FindRenderComponent(LocalComponentRef _ref)
    {
        return const_cast<RenderComponent*>(static_cast<const WorldZone*>(this)->FindRenderComponent(_ref));
    }

    const std::vector<RenderComponent>& GetRenderComponents() const { return m_RenderComponents; };
    const std::vector<BipedComponent>& GetBipedComponents() const { return m_BipedComponents; };
    const std::vector<ColliderComponent>& GetColliderComponents() const { return m_ColliderComponents; };
    const std::vector<FreelookCameraComponent>& GetCameraComponents() const { return m_CameraComponents; };
    const std::vector<WorldObject>& GetWorldObjects() const { return m_WorldObjects; };

    std::vector<RenderComponent>& GetRenderComponents() { return m_RenderComponents; };
    std::vector<BipedComponent>& GetBipedComponents() { return m_BipedComponents; };
    std::vector<ColliderComponent>& GetColliderComponents() { return m_ColliderComponents; };
    std::vector<FreelookCameraComponent>& GetCameraComponents() { return m_CameraComponents; };
    std::vector<WorldObject>& GetWorldObjects() { return m_WorldObjects; };

    const TerrainComponent& GetTerrainComponent() const { return m_TerrainComponent; };
    TerrainComponent& GetTerrainComponent() { return m_TerrainComponent; };

    glm::ivec3 ComputeRelativeCoordinates(glm::vec3 _zonePosition) const;

    bool ContainsPlayer() const;

private:
    bool DestroyWorldObject_Internal(WorldObjectID _objectID, bool _removedFromWorld);

private:
    static constexpr u32 INITIAL_WORLDOBJECT_COUNT = 64;
    static constexpr u32 INITIAL_COMPONENT_COUNT = 32;
    static constexpr u32 MAX_WORLDOBJECT_COUNT = 128;
    static constexpr u32 MAX_COMPONENT_COUNT = 128;

    glm::ivec3 m_Coordinates;
    glm::vec3 m_Dimensions;

    TerrainComponent m_TerrainComponent;

    World* m_OwnerWorld;

    std::vector<WorldObject>               m_WorldObjects;

    std::vector<BipedComponent>            m_BipedComponents;
    std::vector<ColliderComponent>         m_ColliderComponents;
    std::vector<FreelookCameraComponent>   m_CameraComponents;
    std::vector<RenderComponent>           m_RenderComponents;
};