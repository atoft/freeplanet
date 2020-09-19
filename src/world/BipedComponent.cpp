//
// Created by alastair on 26/09/17.
//

#include "BipedComponent.h"
#include "ComponentAccess.h"

#include <src/engine/Engine.h>
#include <src/profiling/Profiler.h>
#include <src/graphics/ui/UIDisplay.h>
#include <src/world/WorldObject.h>
#include <src/world/World.h>
#include <src/world/collision/CollisionHandler.h>

BipedComponent::BipedComponent(World* _world, WorldObjectID _ownerID)
{
    m_World = _world;
    m_WorldObjectID = _ownerID;
    m_JumpStopwatch = Stopwatch(m_World);
}

// TODO Move logic out of the component and into a handler.
void BipedComponent::Update(TimeMS _delta)
{
    ProfileCurrentFunction();

    if(m_World == nullptr)
    {
        LogError("Player has null world");
        return;
    }

    WorldObject* self = GetOwnerObject();

    if(self == nullptr)
    {
        LogError("Couldn't find player's owner");
        return;
    }

    const glm::vec3 upVector = self->GetUpVector();

    if(m_WalkDirection != glm::vec3())
    {
        m_WalkDirection = glm::normalize(m_WalkDirection);
    }


    const f32 gravity = m_World->GetGravityStrength();
    const f32 walkAcceleration = 20.0f;
    const f32 friction = 20.0f;

    const bool isStanding = IsStandingOnGround(self);

    if (isStanding)
    {
        m_FallSpeed = 0.f;
    }
    else
    {
        m_FallSpeed -= _delta * (Globals::FREEPLANET_IS_GRAVITY ? gravity : 0) / 1000.f;
    }

    m_CanJump = isStanding;

    if(m_IsWalking)
    {
        const f32 maxSpeed = m_IsSprinting ? m_MaxSprintSpeed : m_MaxWalkSpeed;
        m_CurrentWalkSpeed = std::min(m_CurrentWalkSpeed + walkAcceleration * _delta/1000.0f, maxSpeed);
    }
    else
    {
       m_CurrentWalkSpeed = std::max(m_CurrentWalkSpeed - friction * _delta/1000.0f, 0.0f);
    }

    if(m_JumpStopwatch.IsRunning())
    {
        constexpr f32 JUMP_ACCELERATION = 100.f;
        m_FallSpeed += JUMP_ACCELERATION * _delta / 1000.f;
    }
    else if (m_JumpStopwatch.IsFinished())
    {
        m_JumpStopwatch.Stop();
    }

    m_LinearVelocity += (m_CurrentWalkSpeed * m_WalkDirection) + (m_FallSpeed * upVector);

    self->SetPosition(self->GetPosition() + m_LinearVelocity * _delta / 1000.0f);

    m_IsWalking = false;
    m_WalkDirection = glm::vec3();
    m_LinearVelocity = glm::vec3();
    m_IsSprinting = false;
}

void BipedComponent::OnMouseInput(f32 _mouseX, f32 _mouseY)
{
}

void BipedComponent::OnButtonInput(InputType _type)
{
    WorldObject* self = GetOwnerObject();

    constexpr f32 ROLL_SPEED = 0.01f;

    if (_type == InputType::MoveForward)
    {
        m_WalkDirection += self->GetForwardVector();
        m_IsWalking = true;
    }
    else if (_type == InputType::MoveBack)
    {
        m_WalkDirection -= self->GetForwardVector();
        m_IsWalking = true;
    }
    else if (_type == InputType::StrafeRight)
    {
        m_WalkDirection += self->GetRightVector();
        m_IsWalking = true;
    }
    else if (_type == InputType::StrafeLeft)
    {
        m_WalkDirection -= self->GetRightVector();
        m_IsWalking = true;
    }
    else if (_type == InputType::LevitateUp)
    {
        m_WalkDirection += self->GetUpVector();
        m_IsWalking = true;
    }
    else if (_type == InputType::LevitateDown)
    {
        m_WalkDirection -= self->GetUpVector();
        m_IsWalking = true;
    }
    else if (_type == InputType::Jump && m_CanJump)
    {
        m_IsWalking = true;

        constexpr f32 JUMP_DURATION = 0.125f;
        m_JumpStopwatch.Start(JUMP_DURATION);
    }
    else if (_type == InputType::RollRight)
    {
        self->Rotate(glm::vec3(0,0,-1), ROLL_SPEED);
    }
    else if (_type == InputType::RollLeft)
    {
        self->Rotate(glm::vec3(0,0,-1), -ROLL_SPEED);
    }
    else if (_type == InputType::Sprint)
    {
        m_IsSprinting = true;
    }
}

bool BipedComponent::IsStandingOnGround(const WorldObject* self) const
{
    bool isStanding = false;

    const ColliderComponent* collider = ComponentAccess::GetComponent<ColliderComponent>(*self);

    if(collider != nullptr)
    {
        for (const CollisionResult &collision : collider->m_CollisionsLastFrame)
        {
            constexpr f32 REQUIRED_UP_DOT_PRODUCT = 0.50f;

            if (glm::dot(collision.m_Normal, -self->GetUpVector()) > REQUIRED_UP_DOT_PRODUCT)
            {
                isStanding = true;
                break;
            }
        }
    }
    return isStanding;
}

void BipedComponent::DebugDraw(UIDrawInterface& _interface) const
{
    const WorldObject* self = GetOwnerObject();

    std::string debugInfoString = self->GetName()
                                  + "\nPosition: "
                                  + glm::to_string(self->GetPosition())
                                  + "\nRotation: "
                                  + glm::to_string(self->GetRotation())
                                  + "\nWorldZone: "
                                  + glm::to_string(self->GetWorldPosition().m_ZoneCoordinates)
                                  ;

    _interface.DrawString(glm::ivec2(20, 20), debugInfoString, 32.f);
}
