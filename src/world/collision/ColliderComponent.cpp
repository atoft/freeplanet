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

#include "ColliderComponent.h"

#include <src/world/World.h>

ColliderComponent::ColliderComponent(World* world, WorldObjectID _ownerID, CollisionPrimitiveType _primitiveType, MovementType _movementType)
{
    m_World = world;
    m_WorldObjectID = _ownerID;
    m_MovementType = _movementType;
    m_CollisionPrimitiveType = _primitiveType;
    m_Bounds = AABB(glm::vec3(0.5f), glm::vec3());
}


