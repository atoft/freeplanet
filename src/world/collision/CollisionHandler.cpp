//
// Created by alastair on 22/12/18.
//

#include "CollisionHandler.h"

#include <src/engine/Engine.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/World.h>
#include <src/world/collision/CollisionAlgorithms.h>
#include <src/world/collision/CollisionHelpers.h>
#include <src/world/collision/RaycastAlgorithms.h>
#include <src/profiling/Profiler.h>

CollisionHandler::CollisionHandler(World* _world)
    : m_World(_world)
{
}

void CollisionHandler::Update(TimeMS _dt)
{
    ProfileCurrentFunction();

    if(Engine::GetInstance().GetCommandLineArgs().m_Noclip)
    {
        return;
    }

    for(WorldZone& zone : m_World->GetActiveZones())
    {
        if(zone.GetColliderComponents().empty())
        {
            continue;
        }

        // Prepare colliders
        for(ColliderComponent& collider : zone.GetColliderComponents())
        {
            collider.m_CollisionsLastFrame.clear();
        }

        // Collision between WorldObjects
        for(u32 firstIdx = 0; firstIdx < zone.GetColliderComponents().size() - 1; ++firstIdx)
        {
            for(u32 secondIdx = firstIdx + 1; secondIdx < zone.GetColliderComponents().size(); ++secondIdx)
            {
                ColliderComponent& collider1 = zone.GetColliderComponents()[firstIdx];
                ColliderComponent& collider2 = zone.GetColliderComponents()[secondIdx];

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

                    object1->SetPosition(object1->GetPosition() + object1Distance * collision->m_Normal);
                    object2->SetPosition(object2->GetPosition() - object2Distance * collision->m_Normal);

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
        for(ColliderComponent& collider : zone.GetColliderComponents())
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
                    terrainComponent.GetChunks(),
                    MathsHelpers::GetPosition(zone.GetTerrainModelTransform()));

            if(collision != std::nullopt)
            {
                //static u32 i = 0;
                //LogMessage(std::to_string(i++) + "Collision between " + object->GetName() + " and terrain");

                object->SetPosition(object->GetPosition() + collision->m_Normal * collision->m_Distance);

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
        return CollisionAlgorithms::CollideOBB_OBB(
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
                                                             const std::vector<TerrainChunk>& _terrain,
                                                             const glm::vec3 _terrainOffset)
{
    if(_collider.m_CollisionPrimitiveType == CollisionPrimitiveType::OBB)
    {
        return CollisionAlgorithms::CollideOBB_Terrain(
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

std::optional<f32> CollisionHandler::DoRaycast(WorldPosition _origin, glm::vec3 _direction)
{
    ProfileCurrentFunction();

    _direction = glm::normalize(_direction);

    f32 minDistance = std::numeric_limits<f32>::max();
    bool foundCollision = false;

    // TODO Raycast to objects

    for (const WorldZone& zone : m_World->GetActiveZones())
    {
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
