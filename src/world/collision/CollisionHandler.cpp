//
// Created by alastair on 22/12/18.
//

#include "CollisionHandler.h"

#include <src/graphics/ui/UIDrawInterface.h>
#include <src/profiling/Profiler.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/collision/algorithms/CollideOBBOBB.h>
#include <src/world/collision/algorithms/CollideOBBTerrain.h>
#include <src/world/collision/CollisionHelpers.h>
#include <src/world/collision/RaycastAlgorithms.h>
#include <src/world/World.h>
#include <src/world/planet/PlanetGeneration.h>

CollisionHandler::CollisionHandler(World* _world)
    : m_World(_world)
{
}

void CollisionHandler::Update(TimeMS _dt)
{
    ProfileCurrentFunction();

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

            if (collider.m_KeepUpright && m_World->GetPlanet() != nullptr)
            {
                WorldObject* owner = collider.GetOwnerObject();

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
        }

        // Collision between WorldObjects
        for(u32 firstIdx = 0; firstIdx < zone.GetComponents<ColliderComponent>().size() - 1; ++firstIdx)
        {
            for(u32 secondIdx = firstIdx + 1; secondIdx < zone.GetComponents<ColliderComponent>().size(); ++secondIdx)
            {
                ColliderComponent& collider1 = zone.GetComponents<ColliderComponent>()[firstIdx];
                ColliderComponent& collider2 = zone.GetComponents<ColliderComponent>()[secondIdx];

                if(   collider1.m_MovementType == MovementType::Fixed
                   && collider2.m_MovementType == MovementType::Fixed)
                {
                    // No point testing for collisions if we can't resolve.
                    continue;
                }

                if(   !collider1.m_CollisionEnabled
                   || !collider2.m_CollisionEnabled)
                {
                    // No point testing for collisions if either of the pair has been disabled.
                    continue;
                }

                WorldObject* object1 = collider1.GetOwnerObject();
                WorldObject* object2 = collider2.GetOwnerObject();
                assert(object1 != nullptr && object2 != nullptr);

                std::optional<CollisionResult> collision = DoCollision(collider1, *object1, collider2, *object2);

                if(collision != std::nullopt)
                {
                    const f32 object1Movability = collider1.m_MovementType == MovementType::Movable ? 1.f : 0.f;
                    const f32 object2Movability = collider2.m_MovementType == MovementType::Movable ? 1.f : 0.f;

                    const f32 object1Distance = object1Movability * collision->m_Distance * collider1.m_MassScale /
                                          ((collider1.m_MassScale * object1Movability) +
                                           (collider2.m_MassScale * object2Movability));
                    const f32 object2Distance = object2Movability * collision->m_Distance * collider2.m_MassScale /
                                          ((collider1.m_MassScale * object1Movability) +
                                           (collider2.m_MassScale * object2Movability));

                    if (m_ShouldResolveCollisions)
                    {
                        object1->SetPosition(object1->GetPosition() + object1Distance * collision->m_Normal);
                        object2->SetPosition(object2->GetPosition() - object2Distance * collision->m_Normal);
                    }

                    const CollisionResult collider1Result = *collision;

                    // Normal is in the opposite direction for the second of the pair.
                    CollisionResult collider2Result = *collision;
                    collider2Result.m_Normal = - collider2Result.m_Normal;

                    collider1.m_CollisionsLastFrame.push_back(collider1Result);
                    collider2.m_CollisionsLastFrame.push_back(collider2Result);
                }
            }
        }

        // Collision with terrain
        for(ColliderComponent& collider : zone.GetComponents<ColliderComponent>())
        {
            if(collider.m_MovementType == MovementType::Fixed)
            {
                continue;
            }

            if(!collider.m_CollisionEnabled)
            {
                // No point testing for collisions if either of the pair has been disabled.
                continue;
            }

            WorldObject* object = collider.GetOwnerObject();
            assert(object != nullptr);

            const TerrainComponent& terrainComponent = zone.GetTerrainComponent();

            std::optional<CollisionResult> collision = DoCollision(
                    collider,
                    *object,
                    terrainComponent,
                    MathsHelpers::GetPosition(zone.GetTerrainModelTransform()));

            if(collision != std::nullopt)
            {
                if (m_ShouldResolveCollisions)
                {
                    object->SetPosition(object->GetPosition() + collision->m_Normal * collision->m_Distance);
                }

                collider.m_CollisionsLastFrame.push_back(*collision);
            }
        }

        // TODO Keep track of objects crossing the WorldZone boundaries
    }

    // TODO Resolve collisions across boundaries
}

std::optional<CollisionResult> CollisionHandler::DoCollision(const ColliderComponent& _collider1,
        const WorldObject& _object1,
        const ColliderComponent& _collider2,
        const WorldObject& _object2)
{
    if(   _collider1.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB
       && _collider2.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB)
    {
        return CollideOBBOBB::Collide(
                _object1.GetZoneTransform(),
                _collider1.m_Bounds,
                _object2.GetZoneTransform(),
                _collider2.m_Bounds);
    }
    else
    {
        LogWarning("Unsupported collision (" + _object1.GetName() + " and " + _object2.GetName() + ") was skipped.");
        return std::nullopt;
    }
}

std::optional<CollisionResult> CollisionHandler::DoCollision(const ColliderComponent& _collider,
                                                             const WorldObject& _object,
                                                             const TerrainComponent& _terrain,
                                                             const glm::vec3 _terrainOffset)
{
    if(_collider.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB)
    {
        return CollideOBBTerrain::Collide(
                _object.GetZoneTransform(),
                _collider.m_Bounds,
                _terrain,
                _terrainOffset);
    }
    else
    {
        LogWarning("Unsupported collision (" + _object.GetName() + " and terrain) was skipped.");
        return std::nullopt;
    }
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
                        FontStyle::Sans,
                        true);

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
            16.f,
            Color(0.f,0.f,0.f,1.f),
            FontStyle::Sans,
            true);
}
