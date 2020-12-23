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

#pragma once

#include "WorldObjectComponent.h"

#include <src/engine/Stopwatch.h>

class BipedComponent : public WorldObjectComponent
{
public:
    BipedComponent(World* _world, WorldObjectID _ownerID);

    bool IsStandingOnGround(const WorldObject* _self) const;

    glm::vec3 m_LinearVelocity = glm::vec3();

    glm::vec3 m_WalkDirection = glm::vec3();

    f32 m_CurrentWalkSpeed = 0.0f;
    f32 m_MaxWalkSpeed = 6.0f;
    f32 m_MaxSprintSpeed = 20.0f;

    f32 m_FallSpeed = 0.0f;

    bool m_IsWalking = false;
    bool m_CanJump = true;
    bool m_IsSprinting = false;

    Stopwatch m_JumpStopwatch;
};
