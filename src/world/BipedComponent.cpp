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

    // TODO Only do the raycast if an action is requested. For now it's every frame for the debug display.
    const FreelookCameraComponent* camera = ComponentAccess::GetComponent<FreelookCameraComponent>(*self);

    const glm::vec3 lookDirection = camera->GetLookDirection();
    std::optional<f32> rayIntersect = m_World->GetCollisionHandler()->DoRaycast(self->GetWorldPosition(), lookDirection);

    if (rayIntersect.has_value())
    {
        m_DebugLastRaycastDistance = *rayIntersect;
    }
    else
    {
        m_DebugLastRaycastDistance = -1.f;
    }

    if (m_RequestedAction != BipedAction::None && rayIntersect.has_value())
    {
        WorldEvent terrainEvent;

        if (m_RequestedAction == BipedAction::AddTerrain)
        {
            terrainEvent.m_Type = WorldEvent::Type::AddTerrain;
        }
        else if (m_RequestedAction == BipedAction::RemoveTerrain)
        {
            terrainEvent.m_Type = WorldEvent::Type::RemoveTerrain;
        }

        constexpr f32 TERRAIN_INTERACTION_RADIUS = 2.f;

        terrainEvent.m_Radius = TERRAIN_INTERACTION_RADIUS;
        terrainEvent.m_Source = GetOwner();
        terrainEvent.m_TargetPosition = self->GetWorldPosition() + lookDirection * (*rayIntersect);

        m_World->AddWorldEvent(terrainEvent);

        m_RequestedAction = BipedAction::None;
    }
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
    else if (_type == InputType::Interact)
    {
        m_RequestedAction = BipedAction::AddTerrain;
    }
    else if (_type == InputType::InteractAlternate)
    {
        m_RequestedAction = BipedAction::RemoveTerrain;
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
                                  + "\nLook distance: "
                                  + std::to_string(m_DebugLastRaycastDistance);

    _interface.DrawString(glm::ivec2(20, 20), debugInfoString, 32.f);

    const FreelookCameraComponent* camera = ComponentAccess::GetComponent<FreelookCameraComponent>(*self);

    // TODO don't we have a better way to ask for our zone?
    _interface.DebugDrawSphere(GetOwnerObject()->GetWorldPosition().m_ZoneCoordinates,
                               GetOwnerObject()->GetPosition()
                                   + camera->GetLookDirection() * m_DebugLastRaycastDistance,
                               1.f);
}
