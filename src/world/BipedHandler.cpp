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

#include "BipedHandler.h"

#include <src/world/World.h>

BipedHandler::BipedHandler(World *_world)
    : m_World(_world)
{}

void BipedHandler::Update(TimeMS _delta)
{
    for (WorldZone& zone : m_World->GetActiveZones())
    {
        for (BipedComponent& component : zone.GetComponents<BipedComponent>())
        {
            WorldObject* self = component.GetOwnerObject();

            if(self == nullptr)
            {
                LogError("Couldn't find player's owner");
                return;
            }
         
            const glm::vec3 upVector = self->GetUpVector();
         
            if(component.m_WalkDirection != glm::vec3())
            {
                component.m_WalkDirection = glm::normalize(component.m_WalkDirection);
            }
         
         
            const f32 gravity = m_World->GetGravityStrength();
            const f32 walkAcceleration = 20.0f;
            const f32 friction = 20.0f;
         
            const bool isStanding = component.IsStandingOnGround(self);
         
            if (isStanding)
            {
                component.m_FallSpeed = 0.f;
            }
            else
            {
                component.m_FallSpeed -= _delta * (Globals::FREEPLANET_IS_GRAVITY ? gravity : 0) / 1000.f;
            }
         
            component.m_CanJump = isStanding;
         
            if(component.m_IsWalking)
            {
                const f32 maxSpeed = component.m_IsSprinting ? component.m_MaxSprintSpeed : component.m_MaxWalkSpeed;
                component.m_CurrentWalkSpeed = std::min(component.m_CurrentWalkSpeed + walkAcceleration * _delta/1000.0f, maxSpeed);
            }
            else
            {
               component.m_CurrentWalkSpeed = std::max(component.m_CurrentWalkSpeed - friction * _delta/1000.0f, 0.0f);
            }
         
            if(component.m_JumpStopwatch.IsRunning())
            {
                constexpr f32 JUMP_ACCELERATION = 100.f;
                component.m_FallSpeed += JUMP_ACCELERATION * _delta / 1000.f;
            }
            else if (component.m_JumpStopwatch.IsFinished())
            {
                component.m_JumpStopwatch.Stop();
            }
         
            component.m_LinearVelocity += (component.m_CurrentWalkSpeed * component.m_WalkDirection) + (component.m_FallSpeed * upVector);
         
            self->SetPosition(self->GetPosition() + component.m_LinearVelocity * _delta / 1000.0f);
         
            component.m_IsWalking = false;
            component.m_WalkDirection = glm::vec3();
            component.m_LinearVelocity = glm::vec3();
            component.m_IsSprinting = false;
        }
    }
}

void BipedHandler::OnButtonInput(u32 _playerIdx, WorldObject* _controlledWorldObject, InputType _type)
{
    if (_controlledWorldObject == nullptr)
    {
        return;
    }

    BipedComponent* component = ComponentAccess::GetComponent<BipedComponent>(*_controlledWorldObject);

    constexpr f32 ROLL_SPEED = 0.01f;

    if (_type == InputType::MoveForward)
    {
        component->m_WalkDirection += _controlledWorldObject->GetForwardVector();
        component->m_IsWalking = true;
    }
    else if (_type == InputType::MoveBack)
    {
        component->m_WalkDirection -= _controlledWorldObject->GetForwardVector();
        component->m_IsWalking = true;
    }
    else if (_type == InputType::StrafeRight)
    {
        component->m_WalkDirection += _controlledWorldObject->GetRightVector();
        component->m_IsWalking = true;
    }
    else if (_type == InputType::StrafeLeft)
    {
        component->m_WalkDirection -= _controlledWorldObject->GetRightVector();
        component->m_IsWalking = true;
    }
    else if (_type == InputType::LevitateUp)
    {
        component->m_WalkDirection += _controlledWorldObject->GetUpVector();
        component->m_IsWalking = true;
    }
    else if (_type == InputType::LevitateDown)
    {
        component->m_WalkDirection -= _controlledWorldObject->GetUpVector();
        component->m_IsWalking = true;
    }
    else if (_type == InputType::Jump && component->m_CanJump)
    {
        component->m_IsWalking = true;

        constexpr f32 JUMP_DURATION = 0.125f;
        component->m_JumpStopwatch.Start(JUMP_DURATION);
    }
    else if (_type == InputType::RollRight)
    {
        _controlledWorldObject->Rotate(glm::vec3(0,0,-1), ROLL_SPEED);
    }
    else if (_type == InputType::RollLeft)
    {
        _controlledWorldObject->Rotate(glm::vec3(0,0,-1), -ROLL_SPEED);
    }
    else if (_type == InputType::Sprint)
    {
        component->m_IsSprinting = true;
    }
}

