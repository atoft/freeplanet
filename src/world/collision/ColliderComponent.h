//
// Created by alastair on 16/09/17.
//

#pragma once

#include <src/world/collision/CollisionHelpers.h>
#include <src/world/collision/CollisionPrimitiveType.h>
#include <src/world/GeometryTypes.h>
#include <src/world/WorldObjectComponent.h>

class World;

enum class MovementType
{
    Fixed,
    Movable
};

class ColliderComponent : public WorldObjectComponent
{
    friend class CollisionHandler;

public:
    ColliderComponent(World* world, WorldObjectID _ownerID, CollisionPrimitiveType _primitiveType, MovementType _movementType = MovementType::Fixed);

    AABB m_Bounds;
    float m_MassScale = 1.f;

    bool m_CollisionEnabled = true;
    bool m_KeepUpright = false;
    CollisionPrimitiveType m_CollisionPrimitiveType = CollisionPrimitiveType::AABB;
    MovementType m_MovementType = MovementType::Fixed;
    std::vector<CollisionResult> m_CollisionsLastFrame;
};
