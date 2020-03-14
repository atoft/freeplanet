//
// Created by alastair on 13/07/18.
//

#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "globals.h"

class MathsHelpers
{
public:
    static glm::mat4x4 GetRotationMatrix(glm::mat4x4 _transform);

    static glm::vec3 GetScale(glm::mat4x4 _transform);

    static void SetScale(glm::mat4x4& _inOutTransform, glm::vec3 _scale);

    static glm::vec3 GetPosition(glm::mat4x4 _transform);

    static glm::vec3 GetForwardVector(glm::mat4x4 _transform);

    static glm::vec3 GetRightVector(glm::mat4x4 _transform);

    static glm::vec3 GetUpVector(glm::mat4x4 _transform);

    static bool IsNaN(f32 _value);

    static bool IsNormalized(glm::vec3 _value);

    static glm::vec3 ComputeFaceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

    static f32 ComputeFaceArea(glm::vec3 p1, glm::vec3 p2);

    static glm::mat4x4 GenerateRotationMatrixFromNormal(glm::vec3 normal);

    template <typename Vec, typename Numeric>
    static bool EqualWithEpsilon(const Vec& _lhs, const Vec& _rhs, Numeric _epsilon)
    {
        return glm::all(glm::epsilonEqual(_lhs, _rhs, _epsilon));
    }
};
