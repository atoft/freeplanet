//
// Created by alastair on 07/04/17.
//

#pragma once

#include <glm/vec3.hpp>
#include <type_traits>
#include <vector>

#include <src/engine/InputTypes.h>
#include <src/tools/globals.h>
#include <src/world/GeometryTypes.h>
#include <src/world/WorldObjectID.h>
#include <src/world/WorldObjectRef.h>
#include <src/world/WorldPosition.h>
#include <src/world/ComponentConstants.h>

class WorldZone;
class World;
class WorldObjectComponent;
class RenderComponent;
class LightComponent;
class BipedComponent;
class ColliderComponent;
class FreelookCameraComponent;

// Any object which exists in 3D space
class WorldObject
{
public:
    template<typename T>
    const LocalComponentRef& GetComponentRef() const
    {
        if constexpr (std::is_same<T, BipedComponent>())
        {
            return m_PlayerComponentRef;
        }
        else if constexpr (std::is_same<T, ColliderComponent>())
        {
            return m_CollisionComponentRef;
        }
        else if constexpr (std::is_same<T, FreelookCameraComponent>())
        {
            return m_CameraComponentRef;
        }
        else // if constexpr (std::is_same<T, RenderComponent>())
        {
            return m_RenderComponentRef;
        }
        static_assert(ComponentConstants::ComponentCount == 4);
    }

    template<typename T>
    LocalComponentRef& GetComponentRef()
    {
        return const_cast<LocalComponentRef&>(static_cast<const WorldObject*>(this)->GetComponentRef<T>());
    }

    const WorldObjectRef& GetRef() const
    {
        return m_SelfRef;
    }

    WorldObjectRef& GetRef()
    {
        return m_SelfRef;
    }

    WorldObjectID GetWorldObjectID() const
    {
        return m_WorldObjectID;
    }

    void SetWorldObjectID(WorldObjectID _objectID)
    {
        m_WorldObjectID = _objectID;
    }

    explicit WorldObject(World *_ownerWorld);

    const glm::mat4x4& GetZoneTransform() const { return m_ZoneTransform; };
    glm::mat4x4& GetZoneTransform() { return m_ZoneTransform; };

    glm::vec3 GetPosition() const;
    glm::vec3 GetPreviousPosition() const;

    WorldPosition GetWorldPosition() const;

    void SetPosition(const glm::vec3& _position);

    const glm::vec3 GetRotation() const;

    const glm::vec3 GetForwardVector() const;

    void Rotate(const glm::vec3 &_axis, f32 _angle);

    const glm::vec3 GetScale() const;

    void SetScale(const glm::vec3 &_scale);

    void Update(TimeMS _delta);

    const glm::vec3 GetRightVector() const;

    void SetName(std::string _name);
    const std::string& GetName() const;

    void SetInitialPosition(const glm::vec3 &_position);

    const glm::vec3 GetUpVector() const;

    WorldZone* GetWorldZone();
    const WorldZone* GetWorldZone() const;

private:
    std::string m_Name;

    WorldObjectID m_WorldObjectID;

    WorldObjectRef m_SelfRef;

    World* m_World;

    glm::mat4 m_ZoneTransform;
    glm::mat4 m_PreviousZoneTransform;

    LocalComponentRef m_PlayerComponentRef = REF_INVALID;
    LocalComponentRef m_CollisionComponentRef = REF_INVALID;
    LocalComponentRef m_CameraComponentRef = REF_INVALID;
    LocalComponentRef m_RenderComponentRef = REF_INVALID;
    static_assert(ComponentConstants::ComponentCount == 4);
};
