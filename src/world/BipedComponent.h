//
// Created by alastair on 26/09/17.
//

#pragma once

#include "WorldObjectComponent.h"

#include <src/engine/Stopwatch.h>

class BipedComponent : public WorldObjectComponent
{
public:
    BipedComponent(World* _world, WorldObjectID _ownerID);

    void OnButtonInput(InputType _type) override;
    void OnMouseInput(float _mouseX, float _mouseY) override;
    void Update(TimeMS _delta) override;
    void DebugDraw(UIDrawInterface& _interface) const override;

private:
    bool IsStandingOnGround(const WorldObject* self) const;

private:
    // For now all player gameplay logic is in here. In future it will probably move out
    // to a separate component and only locomotion-related logic will stay in this component.
    enum class BipedAction
    {
        None,
        AddTerrain,
        RemoveTerrain
    };

    glm::vec3 m_LinearVelocity = glm::vec3();

    glm::vec3 m_WalkDirection = glm::vec3();

    f32 m_CurrentWalkSpeed = 0.0f;
    f32 m_MaxWalkSpeed = 6.0f;
    f32 m_MaxSprintSpeed = 20.0f;

    f32 m_FallSpeed = 0.0f;

    bool m_IsWalking = false;
    bool m_CanJump = true;
    bool m_IsSprinting = false;
    BipedAction m_RequestedAction = BipedAction::None;

    Stopwatch m_JumpStopwatch;

    f32 m_DebugLastRaycastDistance = 0.f;
};
