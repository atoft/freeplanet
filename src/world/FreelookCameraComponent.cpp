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
}

f32 FreelookCameraComponent::GetFarClipDistance() const
{
    return Globals::FREEPLANET_FAR_CLIP_DISTANCE;
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
