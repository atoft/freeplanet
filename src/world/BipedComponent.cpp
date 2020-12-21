//
// Created by alastair on 26/09/17.
//

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

