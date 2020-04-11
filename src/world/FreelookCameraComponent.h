//
// Created by alastair on 26/09/17.
//

#pragma once

#include "WorldObjectComponent.h"

class FreelookCameraComponent : public WorldObjectComponent
{
public:
    FreelookCameraComponent(World *ownerWorld, WorldObjectID _ownerID);

    void OnButtonInput(InputType type) override {};
    void OnMouseInput(float _mouseX, float _mouseY) override;
    void OnRemovedFromWorld() override;
    void Update(TimeMS _delta) override;

    float GetFieldOfView() const;
    float GetFarClipDistance() const;

    const WorldObject* GetParentWorldObject() const;

    glm::mat4 GetCameraZoneTransform() const;
    glm::vec3 GetLookDirection() const;

private:
    glm::vec3 m_AngularVelocity = glm::vec3();

    f32 m_VerticalRotation = 0.f;
};
