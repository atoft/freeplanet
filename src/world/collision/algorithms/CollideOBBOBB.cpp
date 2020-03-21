//
// Created by alastair on 21/03/2020.
//

#include "CollideOBBOBB.h"

std::optional<CollisionResult> CollideOBBOBB::Collide(const glm::mat4& _transform1, const AABB& _bounds1, const glm::mat4& _transform2, const AABB& _bounds2)
{
    const CollisionHelpers::OBBProperties obb1 = CollisionHelpers::GetOBBProperties(_transform1, _bounds1, glm::vec3());
    const CollisionHelpers::OBBProperties obb2 = CollisionHelpers::GetOBBProperties(_transform2, _bounds2, glm::vec3());

    glm::vec3 chosenAxis = glm::vec3();
    f32 penetration = std::numeric_limits<f32>::max();

    // Collision detection in axes of OBB1  x3
    if(
             !TestAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_X, penetration, chosenAxis)
          || !TestAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_Y, penetration, chosenAxis)
          || !TestAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_Z, penetration, chosenAxis))
    {
        return std::nullopt;
    }

    // Collision detection in axes of OBB2  x3
    if(
             !TestAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb2.m_X, penetration, chosenAxis)
          || !TestAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb2.m_Y, penetration, chosenAxis)
          || !TestAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb2.m_Z, penetration, chosenAxis))
    {
        return std::nullopt;
    }

    // Collision detection in axes of cross products of edges of OBB1 and OBB2  x9
    if(
             !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_X, obb2.m_X, penetration, chosenAxis)
          || !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_X, obb2.m_Y, penetration, chosenAxis)
          || !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_X, obb2.m_Z, penetration, chosenAxis)

          || !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_Y, obb2.m_X, penetration, chosenAxis)
          || !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_Y, obb2.m_Y, penetration, chosenAxis)
          || !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_Y, obb2.m_Z, penetration, chosenAxis)

          || !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_Z, obb2.m_X, penetration, chosenAxis)
          || !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_Z, obb2.m_Y, penetration, chosenAxis)
          || !TestCrossProductAxis(obb1.m_Coordinates, obb2.m_Coordinates, obb1.m_Z, obb2.m_Z, penetration, chosenAxis))
    {
        return std::nullopt;
    }

    CollisionResult result;
    result.m_Normal = chosenAxis;
    result.m_Distance = -penetration;

    return result;
}

bool CollideOBBOBB::TestAxis(
        const std::array<glm::vec3, 8>& _obb1,
        const std::array<glm::vec3, 8>& _obb2,
        const glm::vec3& _projectionAxis,
        f32& _inOutPenetration,
        glm::vec3& _inOutAxis)
{
    f32 min1 = std::numeric_limits<f32>::max();
    f32 max1 = -std::numeric_limits<f32>::max();

    f32 min2 = std::numeric_limits<f32>::max();
    f32 max2 = -std::numeric_limits<f32>::max();

    assert(!glm::isnan(_projectionAxis.x));
    assert(!glm::isnan(_projectionAxis.y));
    assert(!glm::isnan(_projectionAxis.z));

    for(u32 pointIdx = 0; pointIdx < 8; ++pointIdx)
    {
        const f32 distOnAxis1 = glm::dot(_obb1[pointIdx], _projectionAxis);

        // If further along the axis in + or - dir, set as our max or min
        max1 = glm::max(max1, distOnAxis1);
        min1 = glm::min(min1, distOnAxis1);

        const f32 distOnAxis2 = glm::dot(_obb2[pointIdx], _projectionAxis);

        // If further along the axis in + or - dir, set as our max or min
        max2 = glm::max(max2, distOnAxis2);
        min2 = glm::min(min2, distOnAxis2);
    }

    const f32 span = glm::max(max1, max2) - glm::min(min1, min2);
    const f32 range = (max1 - min1) + (max2 - min2);

    if (range >= span)
    {
        const f32 penetration = range - span;

        assert(penetration >= 0.f);
        if (penetration < _inOutPenetration)
        {
            // Always want a normal pointed towards the intersecting triangle.
            const glm::vec3 direction = max1 < max2 ? _projectionAxis : -_projectionAxis;

            _inOutPenetration = penetration;
            _inOutAxis = direction;
        }

        return true;
    }

    return false;
}

bool CollideOBBOBB::TestCrossProductAxis(
        const std::array<glm::vec3, 8>& _obb1,
        const std::array<glm::vec3, 8>& _obb2,
        const glm::vec3& _axisA,
        const glm::vec3& _axisB,
        f32& _inOutPenetration,
        glm::vec3& _inOutAxis)
{
    if (glm::abs(glm::dot(_axisA, _axisB)) > 0.99f)
    {
        // The axes are close to parallel, so skip this test.
        return true;
    }

    const glm::vec3 testAxis = glm::normalize(glm::cross(_axisA, _axisB));

    return TestAxis(_obb1, _obb2, testAxis, _inOutPenetration, _inOutAxis);
}
