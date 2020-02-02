//
// Created by alastair on 07/04/17.
//

#include "WorldObject.h"

#include <glm/gtx/matrix_decompose.hpp>

#include <src/world/World.h>
#include <src/tools/MathsHelpers.h>

WorldZone* WorldObject::GetWorldZone()
{
    // We could cache this if performance becomes an issue (we know when during a frame the list of zones
    // could change
    WorldZone* zone = m_World->FindZoneAtCoordinates(m_SelfRef.m_ZoneCoordinates);
    assert(zone != nullptr);

    return zone;
}

const WorldZone* WorldObject::GetWorldZone() const
{
    // We could cache this if performance becomes an issue (we know when during a frame the list of zones
    // could change
    WorldZone* zone = m_World->FindZoneAtCoordinates(m_SelfRef.m_ZoneCoordinates);
    assert(zone != nullptr);

    return zone;
}

glm::vec3 WorldObject::GetPosition() const
{
    return MathsHelpers::GetPosition(m_ZoneTransform);
}

void WorldObject::SetPosition(const glm::vec3 &m_Position)
{
    //WorldObject::m_Position = m_Position;
    m_ZoneTransform[3][0] = m_Position.x / m_ZoneTransform[3][3];
    m_ZoneTransform[3][1] = m_Position.y / m_ZoneTransform[3][3];
    m_ZoneTransform[3][2] = m_Position.z / m_ZoneTransform[3][3];
}

void WorldObject::SetInitialPosition(const glm::vec3 &_position)
{
    SetPosition(_position);

    m_PreviousZoneTransform[3][0] = _position.x ;
    m_PreviousZoneTransform[3][1] = _position.y ;
    m_PreviousZoneTransform[3][2] = _position.z ;
}

const glm::vec3 WorldObject::GetRotation() const
{
    return glm::eulerAngles(glm::quat_cast(m_ZoneTransform));
}

void WorldObject::Rotate(const glm::vec3& _axis, f32 _angle)
{
    m_ZoneTransform = glm::rotate(m_ZoneTransform, _angle, _axis);
}

const glm::vec3 WorldObject::GetScale() const
{
    return MathsHelpers::GetScale(m_ZoneTransform);
}

void WorldObject::SetScale(const glm::vec3& _scale)
{
    MathsHelpers::SetScale(m_ZoneTransform, _scale);
}

const glm::vec3 WorldObject::GetForwardVector() const
{
    return glm::vec4(0,0,-1.f, 1) * glm::inverse(MathsHelpers::GetRotationMatrix(m_ZoneTransform));
}

const glm::vec3 WorldObject::GetRightVector() const
{
    return glm::vec4(1,0,0, 1) * glm::inverse(MathsHelpers::GetRotationMatrix(m_ZoneTransform));

}

const glm::vec3 WorldObject::GetUpVector() const
{
    return glm::vec4(0,1,0, 1) * glm::inverse(MathsHelpers::GetRotationMatrix(m_ZoneTransform));
}

void WorldObject::Update(TimeMS _delta)
{
    m_PreviousZoneTransform = m_ZoneTransform;

    for(u32 idx = 0; idx < 4; ++idx)
    {
        for(u32 vecIdx = 0; vecIdx < 4; ++vecIdx)
        {
            assert(!MathsHelpers::IsNaN(m_ZoneTransform[idx][vecIdx]));
        }
    }

    assert(m_ZoneTransform[3][3] != 0);
    assert(m_ZoneTransform[3][3] == 1.f);
}

void WorldObject::OnMouseInput(float _mouseX, float _mouseY)
{
    LogError("Not implemented.");
}

void WorldObject::OnButtonInput(InputType _type)
{
    LogError("Not implemented.");
}

WorldObject::WorldObject(World *_ownerWorld)
{
    m_Name = "Unnamed WorldObject";

    m_World = _ownerWorld;
    assert(m_World != nullptr);

    m_PlayerComponentRef = REF_INVALID;
    m_CollisionComponentRef  = REF_INVALID;
    m_CameraComponentRef = REF_INVALID;
    m_RenderComponentRef = REF_INVALID;

    m_ZoneTransform = glm::mat4x4(1.f);
    m_ZoneTransform[3][3] = 1.f;
    m_PreviousZoneTransform = glm::mat4x4();
    m_PreviousZoneTransform[3][3] = 1.f;

    SetPosition(glm::vec3(0,0,0));
    SetScale(glm::vec3(1,1,1));
}

void WorldObject::SetName(std::string _name)
{
    m_Name = _name;
}

const std::string &WorldObject::GetName() const
{
    return m_Name;
}

glm::vec3 WorldObject::GetPreviousPosition() const
{
    return m_PreviousZoneTransform[3] * m_PreviousZoneTransform[3][3];
}

WorldPosition WorldObject::GetWorldPosition() const
{
    return { m_SelfRef.m_ZoneCoordinates, GetPosition() };
}

const BipedComponent* WorldObject::GetBipedComponent() const
{
    return GetWorldZone()->FindBipedComponent(m_PlayerComponentRef);
}

const ColliderComponent* WorldObject::GetColliderComponent() const
{
    return GetWorldZone()->FindColliderComponent(m_CollisionComponentRef);
}

const FreelookCameraComponent* WorldObject::GetFreelookCameraComponent() const
{
    return GetWorldZone()->FindCameraComponent(m_CameraComponentRef);
}

const RenderComponent* WorldObject::GetRenderComponent() const
{
    return GetWorldZone()->FindRenderComponent(m_RenderComponentRef);
}

BipedComponent* WorldObject::GetBipedComponent()
{
    return GetWorldZone()->FindBipedComponent(m_PlayerComponentRef);
}

ColliderComponent* WorldObject::GetColliderComponent()
{
    return GetWorldZone()->FindColliderComponent(m_CollisionComponentRef);
}

FreelookCameraComponent* WorldObject::GetFreelookCameraComponent()
{
    return GetWorldZone()->FindCameraComponent(m_CameraComponentRef);
}

RenderComponent* WorldObject::GetRenderComponent()
{
    return GetWorldZone()->FindRenderComponent(m_RenderComponentRef);
}
