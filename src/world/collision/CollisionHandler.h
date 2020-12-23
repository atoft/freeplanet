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

#include <optional>
#include <vector>

#include <src/tools/globals.h>
#include <src/world/WorldObjectID.h>
#include <src/world/GeometryTypes.h>

struct CollisionResult;
class ColliderComponent;
struct WorldPosition;
class World;
class WorldObject;
class TerrainComponent;
class UIDrawInterface;

// Handles ColliderComponents, and updates WorldObjects based on their collisions with terrain and each other.
class CollisionHandler
{
public:
    explicit CollisionHandler(World* _world);

    void Update(TimeMS _dt);

    enum class RaycastRange
    {
        AllLoadedZones,
        InitialZoneOnly
    };

    std::optional<f32> DoRaycast(WorldPosition _origin, glm::vec3 _direction, RaycastRange _range = RaycastRange::AllLoadedZones);
    std::vector<WorldObjectID> DoShapecast(WorldPosition _origin, AABB _aabb) const;

    void SetShouldResolveCollisions(bool _shouldResolve) { m_ShouldResolveCollisions = _shouldResolve; };

    void DebugDraw(UIDrawInterface& _interface) const;

private:
    bool DoCollision(ColliderComponent& _collider1,
                     WorldObject& _object1,
                     ColliderComponent& _collider2,
                     WorldObject& _object2,
                     const glm::vec3& _object1PositionOffset);

    bool DoCollision(ColliderComponent& _collider,
                     WorldObject& _object,
                     const glm::vec3& _objectPositionOffset,
                     const TerrainComponent& _terrain,
                     const glm::vec3& _terrainOffset);

private:
    World* m_World = nullptr;

    bool m_ShouldResolveCollisions = true;
};
