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

#include <vector>

#include <src/tools/globals.h>
#include <src/world/terrain/TerrainComponent.h>
#include <src/world/ComponentConstants.h>
#include <src/world/WorldObject.h>

#include <src/world/BipedComponent.h>
#include <src/world/FreelookCameraComponent.h>
#include <src/world/collision/ColliderComponent.h>
#include <src/world/LightComponent.h>
#include <src/world/particles/ParticleSystemComponent.h>
#include <src/graphics/RenderComponent.h>
#include <src/world/terrain/TerrainComponent.h>

class World;
struct PropRecipe;

class WorldZone
{
public:
    WorldZone(World* _world, glm::ivec3 _position, glm::vec3 _dimensions);

    void Update(TimeMS delta);

    const WorldObject* GetWorldObject(LocalWorldObjectRef _ref) const;
    bool DestroyWorldObject(WorldObjectID _objectID);
    bool TransferWorldObjectOutOfZone(WorldObjectID _objectID);

    glm::ivec3 GetCoordinates() const { return m_Coordinates; };
    glm::vec3 GetDimensions() const { return m_Dimensions; };

    glm::mat4x4 GetGlobalTransform() const;
    glm::mat4x4 GetRelativeTransform(glm::ivec3 _relativeZone) const;
    glm::mat4x4 GetTerrainModelTransform() const;

    void OnRemovedFromWorld();

    template<typename T>
    const T* FindComponent(LocalComponentRef _ref) const
    {
        if(_ref < GetComponents<T>().size())
        {
            return &GetComponents<T>()[_ref];
        }
        return nullptr;
    }

    template<typename T>
    T* FindComponent(LocalComponentRef _ref)
    {
        return const_cast<T*>(static_cast<const WorldZone*>(this)->FindComponent<T>(_ref));
    }

    template<typename T>
    const std::vector<T>& GetComponents() const
    {
        if constexpr (std::is_same<T, BipedComponent>())
        {
            return m_BipedComponents;
        }
        else if constexpr (std::is_same<T, ColliderComponent>())
        {
            return m_ColliderComponents;
        }
        else if constexpr (std::is_same<T, FreelookCameraComponent>())
        {
            return m_CameraComponents;
        }
        else if constexpr (std::is_same<T, RenderComponent>())
        {
            return m_RenderComponents;
        }
        else if constexpr (std::is_same<T, LightComponent>())
        {
            return m_LightComponents;
        }
        else // if constexpr (std::is_same<T, ParticleSystemComponent>())
        {
            return m_ParticleSystemComponents;
        }
        static_assert(ComponentConstants::ComponentCount == 6);
    }

    template<typename T>
    std::vector<T>& GetComponents()
    {
        return const_cast<std::vector<T>&>(static_cast<const WorldZone*>(this)->GetComponents<T>());
    }

    template<typename T, typename... Args>
    T& AddComponent(WorldObject& _object, Args... _args)
    {
        std::vector<T>& components = GetComponents<T>();
        components.emplace_back(m_OwnerWorld, _object.GetWorldObjectID(), _args...);
        _object.GetComponentRef<T>() = components.size() - 1;
        assert(_object.GetWorldZone() == this);

        return components.back();
    }

    const std::vector<WorldObject>& GetWorldObjects() const { return m_WorldObjects; };
    std::vector<WorldObject>& GetWorldObjects() { return m_WorldObjects; };

    const TerrainComponent& GetTerrainComponent() const { return m_TerrainComponent; };
    TerrainComponent& GetTerrainComponent() { return m_TerrainComponent; };

    bool IsProceduralSpawningDone() const { return m_IsProceduralSpawningDone; };
    void SetProceduralSpawningDone() { m_IsProceduralSpawningDone = true; };

    bool ContainsPlayer() const;

    // HACK: Need a better place to do things in World on first load.
    bool m_HasLoadedFromSave = false;
    
private:
    bool DestroyWorldObject_Internal(WorldObjectID _objectID, bool _removedFromWorld);

private:
    static constexpr u32 INITIAL_WORLDOBJECT_COUNT = 64;
    static constexpr u32 INITIAL_COMPONENT_COUNT = 32;

    glm::ivec3 m_Coordinates;
    glm::vec3 m_Dimensions;

    TerrainComponent m_TerrainComponent;

    // TODO probably a zone component containing spawning info.
    bool m_IsProceduralSpawningDone = false;

    World* m_OwnerWorld;

    std::vector<WorldObject>               m_WorldObjects;

    std::vector<BipedComponent>            m_BipedComponents;
    std::vector<ColliderComponent>         m_ColliderComponents;
    std::vector<FreelookCameraComponent>   m_CameraComponents;
    std::vector<RenderComponent>           m_RenderComponents;
    std::vector<LightComponent>            m_LightComponents;
    std::vector<ParticleSystemComponent>   m_ParticleSystemComponents;
    static_assert(ComponentConstants::ComponentCount == 6);
};
