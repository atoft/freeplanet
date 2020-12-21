//
// Created by alastair on 26/09/17.
//

#pragma once

#include "WorldObjectComponent.h"

class FreelookCameraComponent : public WorldObjectComponent
{
public:
    FreelookCameraComponent(World *ownerWorld, WorldObjectID _ownerID);

    f32 GetFieldOfView() const;
    f32 GetFarClipDistance() const;

    glm::mat4 GetCameraZoneTransform() const;
    glm::vec3 GetLookDirection() const;

    glm::vec3 m_AngularVelocity = glm::vec3();
    f32 m_VerticalRotation = 0.f;
};
