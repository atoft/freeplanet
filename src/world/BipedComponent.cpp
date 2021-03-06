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

#include "BipedComponent.h"

#include <src/world/WorldObject.h>
#include <src/world/World.h>
#include <src/world/collision/CollisionHandler.h>

BipedComponent::BipedComponent(World* _world, WorldObjectID _ownerID)
{
    m_World = _world;
    m_WorldObjectID = _ownerID;
    m_JumpStopwatch = Stopwatch(m_World);
}

bool BipedComponent::IsStandingOnGround(const WorldObject* _self) const
{
    bool isStanding = false;

    const ColliderComponent* collider = ComponentAccess::GetComponent<ColliderComponent>(*_self);

    if(collider != nullptr)
    {
        for (const CollisionResult &collision : collider->m_CollisionsLastFrame)
        {
            constexpr f32 REQUIRED_UP_DOT_PRODUCT = 0.50f;

            if (glm::dot(collision.m_Normal, -_self->GetUpVector()) > REQUIRED_UP_DOT_PRODUCT)
            {
                isStanding = true;
                break;
            }
        }
    }
    return isStanding;
}

