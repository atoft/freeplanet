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
