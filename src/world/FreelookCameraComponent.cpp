//
// Created by alastair on 26/09/17.
//

#include "FreelookCameraComponent.h"

#include <src/tools/MathsHelpers.h>
#include <src/profiling/Profiler.h>
#include <src/world/WorldObject.h>
#include <src/engine/Engine.h>

FreelookCameraComponent::FreelookCameraComponent(World *ownerWorld, WorldObjectID _ownerID)
{
    m_World = ownerWorld;
    m_WorldObjectID = _ownerID;
    m_MakeActive = false;
}

void FreelookCameraComponent::OnRemovedFromWorld()
{
    Engine::GetInstance().OnCameraDestroyed(m_WorldObjectID);
}

void FreelookCameraComponent::OnMouseInput(float _mouseX, float _mouseY)
{
    float sensX = 2.f;
    float sensY = 2.f;

    m_AngularVelocity = glm::vec3(-1,0,0) * _mouseX * sensX - glm::vec3(0,1,0) * _mouseY * sensY;
}

void FreelookCameraComponent::Update(TimeMS _delta)
{
    ProfileCurrentFunction();

    const glm::vec3 LOCAL_UP = glm::vec3(0,1,0);

    if(m_MakeActive)
    {
        Engine::GetInstance().RequestCameraChange(m_WorldObjectID);
        m_MakeActive = false;
    }

    // Freelook left and right causes the parent object to rotate,
    // but up and down should only rotate the camera view
    WorldObject* self = GetOwnerObject();
    self->Rotate(LOCAL_UP, m_AngularVelocity.x);

    m_VerticalRotation = glm::clamp(m_VerticalRotation + m_AngularVelocity.y, -glm::half_pi<f32>(), glm::half_pi<f32>());
}

float FreelookCameraComponent::GetFieldOfView() const
{
    return Globals::FREEPLANET_FOV_DEGREES;
}

float FreelookCameraComponent::GetFarClipDistance() const
{
    return Globals::FREEPLANET_FAR_CLIP_DISTANCE;
}

const WorldObject* FreelookCameraComponent::GetParentWorldObject() const
{
    return GetOwnerObject();
}

void FreelookCameraComponent::RequestMakeActive()
{
    m_MakeActive = true;
}

glm::mat4 FreelookCameraComponent::GetCameraZoneTransform() const
{
    const WorldObject* self = GetOwnerObject();

    if(self == nullptr)
    {
        LogError("Couldn't find camera's owner");
        return glm::mat4(1.f);
    }

    const glm::vec3 LOCAL_RIGHT = glm::vec3(1,0,0);
    return glm::rotate(self->GetZoneTransform(), m_VerticalRotation, LOCAL_RIGHT);
}

glm::vec3 FreelookCameraComponent::GetLookDirection() const
{
    return MathsHelpers::GetForwardVector(GetCameraZoneTransform());
}
