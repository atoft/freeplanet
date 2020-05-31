//
// Created by alastair on 22/12/18.
//

#include "CollisionHandler.h"

#include <src/graphics/ui/UIDrawInterface.h>
#include <src/profiling/Profiler.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/collision/algorithms/CollideOBBOBB.h>
#include <src/world/collision/algorithms/CollideOBBTriangle.h>
#include <src/world/collision/CollisionHelpers.h>
#include <src/world/collision/RaycastAlgorithms.h>
#include <src/world/World.h>
#include <src/world/planet/PlanetGeneration.h>
#include <src/world/terrain/TerrainConstants.h>

CollisionHandler::CollisionHandler(World* _world)
    : m_World(_world)
{
}

void CollisionHandler::Update(TimeMS _dt)
{
    ProfileCurrentFunction();

    struct DifferentZoneCollisionRequest
    {
        ColliderComponent* m_Collider = nullptr;
        WorldZone* m_OtherZone = nullptr;
        bool m_WithTerrainOnly = false;
    };

    std::vector<DifferentZoneCollisionRequest> requestedCollisions;

    for(WorldZone& zone : m_World->GetActiveZones())
    {
        if(zone.GetComponents<ColliderComponent>().empty())
        {
            continue;
        }

        // Prepare colliders
        for(ColliderComponent& collider : zone.GetComponents<ColliderComponent>())
        {
            collider.m_CollisionsLastFrame.clear();
            WorldObject* owner = collider.GetOwnerObject();

            if (collider.m_KeepUpright && m_World->GetPlanet() != nullptr)
            {
                const glm::vec3 currentUp = owner->GetUpVector();
                const glm::vec3 desiredUp = PlanetGeneration::GetUpDirection(*m_World->GetPlanet(), owner->GetWorldPosition());

                const f32 dotProduct = glm::dot(currentUp, desiredUp);
                if (dotProduct < 1.f - glm::epsilon<f32>())
                {
                    glm::vec3 newZ;

                    if (glm::abs(glm::dot(desiredUp, owner->GetRightVector())) < 0.99f)
                    {
                        newZ = glm::cross(owner->GetRightVector(), desiredUp);
                    }
                    else
                    {
                        // Avoid numerical issues if the desiredUp happens to be very close to the right vector.
                        // Just keep the existing Z basis (which is the opposite direction from the forward).
                        newZ = -owner->GetForwardVector();
                    }

                    const glm::vec3 newX = glm::cross(desiredUp, newZ);

                    MathsHelpers::SetRotationPart(owner->GetZoneTransform(), newX, desiredUp, newZ);
                }
            }

            // Check if the object is overlapping any neighbouring zones.

            // TODO Need to support other collision primitives.
            assert(collider.m_CollisionPrimitiveType == CollisionPrimitiveType::AABB || collider.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB);
            const AABB aabb = CollisionHelpers::GetAABBForOBB(MathsHelpers::GetRotationMatrix(owner->GetZoneTransform()), collider.m_Bounds);

            AABB otherZoneAABB;
            otherZoneAABB.m_Dimensions = glm::vec3(TerrainConstants::WORLD_ZONE_SIZE)  / 2.f;

            for (s32 z = -1; z <= 1; ++z)
            {
                for (s32 y = -1; y <= 1; ++y)
                {
                    for (s32 x = -1; x <= 1; ++x)
                    {
                        // Skip the object's own zone.
                        if (x == 0 && y == 0 && z == 0)
                        {
                            continue;
                        }

                        // Make sure we don't double up object collisions - we only need to test each pair once.
                        const bool isPreviousZone = x < 0 || y < 0 || z < 0;

                        if (CollisionHelpers::IsIntersecting(owner->GetPosition(), aabb, TerrainConstants::WORLD_ZONE_SIZE * glm::vec3(x, y, z), otherZoneAABB))
                        {
                            WorldZone* otherZone = m_World->FindZoneAtCoordinates(zone.GetCoordinates() + glm::ivec3(x, y, z));

                            if (otherZone != nullptr)
                            {
                                requestedCollisions.push_back({&collider, otherZone, isPreviousZone});
                            }
                        }
                    }
                }
            }
        }

        // Collision between WorldObjects.
        for(u32 firstIdx = 0; firstIdx < zone.GetComponents<ColliderComponent>().size() - 1; ++firstIdx)
        {
            for(u32 secondIdx = firstIdx + 1; secondIdx < zone.GetComponents<ColliderComponent>().size(); ++secondIdx)
            {
                ColliderComponent& collider1 = zone.GetComponents<ColliderComponent>()[firstIdx];
                ColliderComponent& collider2 = zone.GetComponents<ColliderComponent>()[secondIdx];

                WorldObject* object1 = collider1.GetOwnerObject();
                WorldObject* object2 = collider2.GetOwnerObject();
                assert(object1 != nullptr && object2 != nullptr);

                DoCollision(collider1, *object1, collider2, *object2, glm::vec3(0.f));
            }
        }

        // Collision with terrain.
        for(ColliderComponent& collider : zone.GetComponents<ColliderComponent>())
        {
            WorldObject* object = collider.GetOwnerObject();
            assert(object != nullptr);

            const TerrainComponent& terrainComponent = zone.GetTerrainComponent();

            DoCollision(
                    collider,
                    *object,
                    glm::vec3(),
                    terrainComponent,
                    MathsHelpers::GetPosition(zone.GetTerrainModelTransform()));
        }
    }


    // Collision with other zones.
    for (const DifferentZoneCollisionRequest& request : requestedCollisions)
    {
        WorldObject* object1 = request.m_Collider->GetOwnerObject();

        const glm::vec3 positionOffset = object1->GetWorldPosition().GetPositionRelativeTo(*request.m_OtherZone) - object1->GetPosition();

        if (!request.m_WithTerrainOnly)
        {
            for (ColliderComponent& otherCollider : request.m_OtherZone->GetComponents<ColliderComponent>())
            {
                WorldObject* object2 = otherCollider.GetOwnerObject();
                assert(object1 != nullptr && object2 != nullptr);

                DoCollision(*request.m_Collider, *object1, otherCollider, *object2, positionOffset);
            }
        }

        const TerrainComponent& terrainComponent = request.m_OtherZone->GetTerrainComponent();

        DoCollision(
                *request.m_Collider,
                *object1,
                positionOffset,
                terrainComponent,
                MathsHelpers::GetPosition(request.m_OtherZone->GetTerrainModelTransform()));
    }
}

bool CollisionHandler::DoCollision(ColliderComponent& _collider1,
        WorldObject& _object1,
        ColliderComponent& _collider2,
        WorldObject& _object2,
        const glm::vec3& _object1PositionOffset)
{
    if(   _collider1.m_MovementType == MovementType::Fixed
       && _collider2.m_MovementType == MovementType::Fixed)
    {
        // No point testing for collisions if we can't resolve.
        return false;
    }

    if(   !_collider1.m_CollisionEnabled
       || !_collider2.m_CollisionEnabled)
    {
        // No point testing for collisions if either of the pair has been disabled.
        return false;
    }

    glm::mat4x4 zoneTransform1 = _object1.GetZoneTransform();
    MathsHelpers::TranslateWorldSpace(zoneTransform1, _object1PositionOffset);

    std::optional<CollisionResult> collision;

    if(   _collider1.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB
       && _collider2.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB)
    {
        collision = CollideOBBOBB::Collide(
                zoneTransform1,
                _collider1.m_Bounds,
                _object2.GetZoneTransform(),
                _collider2.m_Bounds);
    }
    else
    {
        LogWarning("Unsupported collision (" + _object1.GetName() + " and " + _object2.GetName() + ") was skipped.");
        collision = std::nullopt;
    }

    if(collision != std::nullopt)
    {
        const f32 object1Movability = _collider1.m_MovementType == MovementType::Movable ? 1.f : 0.f;
        const f32 object2Movability = _collider2.m_MovementType == MovementType::Movable ? 1.f : 0.f;

        const f32 object1Distance = object1Movability * collision->m_Distance * _collider1.m_MassScale /
                                    ((_collider1.m_MassScale * object1Movability) +
                                     (_collider2.m_MassScale * object2Movability));

        const f32 object2Distance = object2Movability * collision->m_Distance * _collider2.m_MassScale /
                                    ((_collider1.m_MassScale * object1Movability) +
                                     (_collider2.m_MassScale * object2Movability));

        if (m_ShouldResolveCollisions)
        {
            _object1.SetPosition(_object1.GetPosition() + object1Distance * collision->m_Normal);
            _object2.SetPosition(_object2.GetPosition() - object2Distance * collision->m_Normal);
        }

        const CollisionResult collider1Result = *collision;

        // Normal is in the opposite direction for the second of the pair.
        CollisionResult collider2Result = *collision;
        collider2Result.m_Normal = - collider2Result.m_Normal;

        _collider1.m_CollisionsLastFrame.push_back(collider1Result);
        _collider2.m_CollisionsLastFrame.push_back(collider2Result);

        return true;
    }

    return false;
}

bool CollisionHandler::DoCollision(ColliderComponent& _collider,
                                   WorldObject& _object,
                                   const glm::vec3& _objectPositionOffset,
                                   const TerrainComponent& _terrain,
                                   const glm::vec3& _terrainOffset)
{
    if(_collider.m_MovementType == MovementType::Fixed)
    {
        return false;
    }

    if(!_collider.m_CollisionEnabled)
    {
        // No point testing for collisions if either of the pair has been disabled.
        return false;
    }

    glm::mat4x4 zoneTransform = _object.GetZoneTransform();
    MathsHelpers::TranslateWorldSpace(zoneTransform, _objectPositionOffset);

    bool didCollide = false;

    LogMessage(_object.GetName());

    if(_collider.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB)
    {
        const glm::vec3 chunksToZoneOriginOffset = glm::vec3(_terrain.m_ChunkSize * _terrain.m_ChunksPerEdge) /2.f;

        const glm::vec3 localPosition = MathsHelpers::GetPosition(zoneTransform) + _collider.m_Bounds.m_PositionOffset + chunksToZoneOriginOffset;

        // Use the bounds of the collider to only test chunks that could possibly overlap.
        const AABB boundsForOBB = CollisionHelpers::GetAABBForOBB(MathsHelpers::GetRotationMatrix(zoneTransform), _collider.m_Bounds);
        const glm::ivec3 minRegion = glm::floor((localPosition - boundsForOBB.m_Dimensions) / _terrain.m_ChunkSize);
        const glm::ivec3 maxRegion = glm::ceil((localPosition + boundsForOBB.m_Dimensions) / _terrain.m_ChunkSize);

        const glm::ivec3 clampedMinRegion = glm::clamp(minRegion, glm::ivec3(), glm::ivec3(_terrain.m_ChunksPerEdge));
        const glm::ivec3 clampedMaxRegion = glm::clamp(maxRegion, glm::ivec3(), glm::ivec3(_terrain.m_ChunksPerEdge));

        const u32 dimensions = _terrain.GetDimensions();
        CollisionHelpers::OBBProperties obb = CollisionHelpers::GetOBBProperties(zoneTransform, _collider.m_Bounds, _terrainOffset);

        // Resolve collisions with each of the triangles in the region in turn. Note that a different order of resolution
        // would give a different result.
        for (s32 z = clampedMinRegion.z; z < clampedMaxRegion.z; ++z)
        {
            for (s32 y = clampedMinRegion.y; y < clampedMaxRegion.y; ++y)
            {
                for (s32 x = clampedMinRegion.x; x < clampedMaxRegion.x; ++x)
                {
                    const TerrainChunk& chunk = _terrain.GetChunks()[x + y * dimensions + z * dimensions * dimensions];

                    for (u32 triIdx = 0; triIdx < chunk.m_Count; ++triIdx)
                    {
                        const std::optional<CollisionResult> triangleResult = CollideOBBTriangle::Collide(obb, chunk.m_Triangles[triIdx]);

                        if (triangleResult != std::nullopt)
                        {
                            if (m_ShouldResolveCollisions)
                            {
                                _object.SetPosition(_object.GetPosition() + triangleResult->m_Normal * triangleResult->m_Distance);

                                // Recompute these properties for the new position of the object.
                                zoneTransform = _object.GetZoneTransform();
                                MathsHelpers::TranslateWorldSpace(zoneTransform, _objectPositionOffset);
                                obb = CollisionHelpers::GetOBBProperties(zoneTransform, _collider.m_Bounds, _terrainOffset);

                                // Note that in theory the new position of the object could push it outside of the region, causing
                                // missed collisions. In practice it doesn't seem the distance is big enough to cause a problem,
                                // but this might be an area to improve if issues arise.
                            }

                            _collider.m_CollisionsLastFrame.push_back(*triangleResult);
                        }
                    }
                }
            }
        }
    }
    else
    {
        LogWarning("Unsupported collision (" + _object.GetName() + " and terrain) was skipped.");
    }

    return didCollide;
}

std::optional<f32> CollisionHandler::DoRaycast(WorldPosition _origin, glm::vec3 _direction, RaycastRange _range)
{
    ProfileCurrentFunction();

    _direction = glm::normalize(_direction);

    f32 minDistance = std::numeric_limits<f32>::max();
    bool foundCollision = false;

    // TODO Raycast to objects

    for (const WorldZone& zone : m_World->GetActiveZones())
    {
        if (_range == RaycastRange::InitialZoneOnly)
        {
            if (zone.GetCoordinates() != _origin.m_ZoneCoordinates)
            {
                continue;
            }
        }

        const glm::vec3 localOrigin = _origin.GetPositionRelativeTo(zone) - MathsHelpers::GetPosition(zone.GetTerrainModelTransform());

        // TODO Raycast AABB before terrain

        const TerrainComponent& terrainComponent = zone.GetTerrainComponent();

        const std::optional<RaycastResult> terrainRaycastResult = RaycastAlgorithms::RaycastTerrain(localOrigin, _direction, terrainComponent.GetChunks());

        if (terrainRaycastResult.has_value())
        {
            foundCollision = true;
            minDistance = glm::min(minDistance, terrainRaycastResult->m_Distance);
        }
    }

    if (foundCollision)
    {
        return minDistance;
    }

    return std::nullopt;
}

void CollisionHandler::DebugDraw(UIDrawInterface& _interface) const
{
    u32 collisionCount = 0;
    for (const WorldZone& zone : m_World->GetActiveZones())
    {
        for (const ColliderComponent& collider : zone.GetComponents<ColliderComponent>())
        {
            const WorldObject* owner = collider.GetOwnerObject();

            for (const CollisionResult& result : collider.m_CollisionsLastFrame)
            {
                const glm::vec3 colliderOrigin = owner->GetPosition() + collider.m_Bounds.m_PositionOffset;

                _interface.DebugDrawArrow(zone.GetCoordinates(), colliderOrigin, 1.f + glm::abs(result.m_Distance), result.m_Normal);
                _interface.DrawString(
                        glm::vec2(20, 40 + collisionCount * 50.f),
                        "Collider: " + owner->GetName() + "\nChosen axis: " + result.m_DebugInfo,
                        16.f,
                        Color(1.f,0.f,0.f,1.f),
                        FontStyle::Sans);

                ++collisionCount;
            }

            if (collider.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB)
            {
                const AABB boundsForOBB = CollisionHelpers::GetAABBForOBB(MathsHelpers::GetRotationMatrix(owner->GetZoneTransform()), collider.m_Bounds);
                _interface.DebugDrawAABB(zone.GetCoordinates(), owner->GetPosition(), boundsForOBB);
            }
        }
    }

    _interface.DrawString(
            glm::vec2(20, 20),
            std::to_string(collisionCount) + " collisions this frame.",
            32.f,
            Color(0.f,0.f,0.f,1.f),
            FontStyle::Sans);
}
