//
// Created by alastair on 22/12/18.
//

#pragma once

#include <optional>
#include <vector>

#include <src/tools/globals.h>

struct CollisionResult;
class ColliderComponent;
struct WorldPosition;
class World;
class WorldObject;
struct TerrainChunk;
class UIDrawInterface;

// Handles ColliderComponents, and updates WorldObjects based on their collisions with terrain and each other.
class CollisionHandler
{
public:
    explicit CollisionHandler(World* _world);

    void Update(TimeMS _dt);

    std::optional<f32> DoRaycast(WorldPosition _origin, glm::vec3 _direction);

    void SetShouldResolveCollisions(bool _shouldResolve) { m_ShouldResolveCollisions = _shouldResolve; };

    void DebugDraw(UIDrawInterface& _interface) const;

private:
    static std::optional<CollisionResult> DoCollision(const ColliderComponent& _collider1,
                                                      const WorldObject& _object1,
                                                      const ColliderComponent& _collider2,
                                                      const WorldObject& _object2);

    static std::optional<CollisionResult> DoCollision(const ColliderComponent& _collider,
                                                      const WorldObject& _object,
                                                      const std::vector<TerrainChunk>& _terrain,
                                                      const glm::vec3 _terrainOffset);

private:
    World* m_World = nullptr;

    bool m_ShouldResolveCollisions = true;
};
