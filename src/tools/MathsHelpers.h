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

    static void SetRotationPart(glm::mat4x4& _inOutTransform, glm::vec3 _basisX, glm::vec3 _basisY, glm::vec3 _basisZ);

    static void SetRotationPart(glm::mat4x4& _inOutTransform, const glm::mat3x3& _rotation);

    static glm::vec3 GetScale(glm::mat4x4 _transform);

    static void SetScale(glm::mat4x4& _inOutTransform, glm::vec3 _scale);

    static glm::vec3 GetPosition(glm::mat4x4 _transform);

    static void SetPosition(glm::mat4x4& _inOutTransform, glm::vec3 _position);

    static void TranslateWorldSpace(glm::mat4x4& _inOutTransform, glm::vec3 _translation);

    static glm::vec3 GetForwardVector(glm::mat4x4 _transform);

    static glm::vec3 GetRightVector(glm::mat4x4 _transform);

    static glm::vec3 GetUpVector(glm::mat4x4 _transform);

    static bool IsNaN(f32 _value);

    static bool IsNormalized(glm::vec3 _value);

    static glm::vec3 ComputeFaceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

    static f32 ComputeFaceArea(glm::vec3 p1, glm::vec3 p2);

    static glm::mat4x4 GenerateRotationMatrixFromRight(glm::vec3 _rightVector);

    static glm::mat3x3 GenerateRotationMatrix3x3FromRight(glm::vec3 _rightVector);

    static glm::mat3x3 GenerateRotationMatrix3x3FromUp(glm::vec3 _upVector);

    static glm::vec3 GenerateNormalFromPitchYaw(f32 _pitch, f32 _yaw);

    static glm::vec3 GenerateArbitraryNormal(glm::vec3 _tangent);
    
    template <typename Vec, typename Numeric>
    static bool EqualWithEpsilon(const Vec& _lhs, const Vec& _rhs, Numeric _epsilon)
    {
        return glm::all(glm::epsilonEqual(_lhs, _rhs, _epsilon));
    }
};
