//
// Created by alastair on 16/09/17.
//

#pragma once

#include <src/world/collision/CollisionAlgorithms.h>
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

    void OnButtonInput(InputType type) override {};
    void OnMouseInput(float mouseX, float mouseY) override {};
    void OnRemovedFromWorld() override {};
    void Update(TimeMS delta) override {};

public:
    AABB m_Bounds;
    float m_MassScale = 1.f;

    bool m_CollisionEnabled = true;
    CollisionPrimitiveType m_CollisionPrimitiveType = CollisionPrimitiveType::AABB;
    MovementType m_MovementType = MovementType::Fixed;
    std::vector<CollisionResult> m_CollisionsLastFrame;
};
