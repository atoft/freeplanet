//
// Created by alastair on 16/09/17.
//

#include "ColliderComponent.h"

#include <src/world/World.h>

ColliderComponent::ColliderComponent(World* world, WorldObjectID _ownerID, CollisionPrimitiveType _primitiveType, MovementType _movementType)
{
    m_World = world;
    m_WorldObjectID = _ownerID;
    m_MovementType = _movementType;
    m_CollisionPrimitiveType = _primitiveType;
    m_Bounds = {glm::vec3(0.5f), glm::vec3()};
}


