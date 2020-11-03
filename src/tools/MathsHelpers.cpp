//
// Created by alastair on 13/07/18.
//

#include "MathsHelpers.h"

#include <glm/gtc/matrix_access.hpp>

glm::mat4x4 MathsHelpers::GetRotationMatrix(glm::mat4x4 _transform)
{
    const glm::vec3 scale = GetScale(_transform);

    glm::mat4x4 result = glm::mat4x4();

    result[0][0] = _transform[0][0] / scale.x;
    result[0][1] = _transform[0][1] / scale.x;
    result[0][2] = _transform[0][2] / scale.x;

    result[1][0] = _transform[1][0] / scale.y;
    result[1][1] = _transform[1][1] / scale.y;
    result[1][2] = _transform[1][2] / scale.y;

    result[2][0] = _transform[2][0] / scale.z;
    result[2][1] = _transform[2][1] / scale.z;
    result[2][2] = _transform[2][2] / scale.z;

    result[3][3] = _transform[3][3];

    return result;

}

void MathsHelpers::SetRotationPart(glm::mat4x4& _inOutTransform, glm::vec3 _basisX, glm::vec3 _basisY, glm::vec3 _basisZ)
{
    const glm::vec3 scale = GetScale(_inOutTransform);

    _inOutTransform[0][0] = _basisX.x * scale.x;
    _inOutTransform[0][1] = _basisX.y * scale.x;
    _inOutTransform[0][2] = _basisX.z * scale.x;

    _inOutTransform[1][0] = _basisY.x * scale.y;
    _inOutTransform[1][1] = _basisY.y * scale.y;
    _inOutTransform[1][2] = _basisY.z * scale.y;

    _inOutTransform[2][0] = _basisZ.x * scale.z;
    _inOutTransform[2][1] = _basisZ.y * scale.z;
    _inOutTransform[2][2] = _basisZ.z * scale.z;
}

void MathsHelpers::SetRotationPart(glm::mat4x4& _inOutTransform, const glm::mat3x3& _rotation)
{
    const glm::vec3 scale = GetScale(_inOutTransform);

    _inOutTransform[0][0] = _rotation[0][0] * scale.x;
    _inOutTransform[0][1] = _rotation[0][1] * scale.x;
    _inOutTransform[0][2] = _rotation[0][2] * scale.x;

    _inOutTransform[1][0] = _rotation[1][0] * scale.y;
    _inOutTransform[1][1] = _rotation[1][1] * scale.y;
    _inOutTransform[1][2] = _rotation[1][2] * scale.y;

    _inOutTransform[2][0] = _rotation[2][0] * scale.z;
    _inOutTransform[2][1] = _rotation[2][1] * scale.z;
    _inOutTransform[2][2] = _rotation[2][2] * scale.z;
}

glm::vec3 MathsHelpers::GetScale(glm::mat4x4 _transform)
{
    return glm::vec3(
            glm::length(glm::vec3(glm::column(_transform, 0))),
            glm::length(glm::vec3(glm::column(_transform, 1))),
            glm::length(glm::vec3(glm::column(_transform, 2))));
}

void MathsHelpers::SetScale(glm::mat4x4& _inOutTransform, glm::vec3 _scale)
{
    const glm::vec3 previousScale = GetScale(_inOutTransform);
    _inOutTransform = glm::scale(_inOutTransform, _scale / previousScale);
}

glm::vec3 MathsHelpers::GetPosition(glm::mat4x4 _transform)
{
    return _transform[3] * _transform[3][3];
}

void MathsHelpers::SetPosition(glm::mat4x4& _inOutTransform, glm::vec3 _position)
{
    _inOutTransform[3][0] = _position.x / _inOutTransform[3][3];
    _inOutTransform[3][1] = _position.y / _inOutTransform[3][3];
    _inOutTransform[3][2] = _position.z / _inOutTransform[3][3];
}

void MathsHelpers::TranslateWorldSpace(glm::mat4x4& _inOutTransform, glm::vec3 _translation)
{
    SetPosition(_inOutTransform, GetPosition(_inOutTransform) + _translation);
}

glm::vec3 MathsHelpers::GetForwardVector(glm::mat4x4 _transform)
{
    return glm::vec4(0,0,-1.f, 1) * glm::inverse(MathsHelpers::GetRotationMatrix(_transform));
}

glm::vec3 MathsHelpers::GetRightVector(glm::mat4x4 _transform)
{
    return glm::vec4(1,0,0, 1) * glm::inverse(MathsHelpers::GetRotationMatrix(_transform));

}

glm::vec3 MathsHelpers::GetUpVector(glm::mat4x4 _transform)
{
    return glm::vec4(0,1,0, 1) * glm::inverse(MathsHelpers::GetRotationMatrix(_transform));
}

bool MathsHelpers::IsNaN(f32 _value)
{
    return _value != _value;
}

bool MathsHelpers::IsNormalized(glm::vec3 _value)
{
    const f32 length = glm::length(_value);
    return length >= 1.f - 2.f * glm::epsilon<f32>() && length <= 1.f + 2.f * glm::epsilon<f32>();
}

glm::vec3 MathsHelpers::ComputeFaceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    const glm::vec3 dir1 = p2 - p1;
    const glm::vec3 dir2 = p3 - p1;

    return glm::cross(dir1, dir2);
}

f32 MathsHelpers::ComputeFaceArea(glm::vec3 p1, glm::vec3 p2)
{
    return glm::length(glm::cross(p1, p2));
}

// Generates an arbitrary rotation matrix based on the normal. Note that when specifying a normal only there are
// infinitely many possible rotation matrices.
glm::mat4x4 MathsHelpers::GenerateRotationMatrixFromRight(glm::vec3 _rightVector)
{
    glm::mat4x4 matrix = GenerateRotationMatrix3x3FromRight(_rightVector);
    matrix[3][3] = 1.f;

    return matrix;
}

glm::mat3x3 MathsHelpers::GenerateRotationMatrix3x3FromRight(glm::vec3 _rightVector)
{
    // Pick an arbitrary vector
    glm::vec3 other = glm::vec3(1,0,0);

    // Make sure it's not parallel
    if(glm::abs(glm::dot(_rightVector, other)) > 0.99f)
    {
        other = glm::vec3(0,1,0);
    }

    glm::vec3 basisY = glm::normalize(glm::cross(_rightVector, other));
    glm::vec3 basisZ = glm::cross(_rightVector, basisY);

    return glm::mat3x3(_rightVector, basisY, basisZ);
}

glm::mat3x3 MathsHelpers::GenerateRotationMatrix3x3FromUp(glm::vec3 _upVector)
{
    // Pick an arbitrary vector
    glm::vec3 other = glm::vec3(0,1,0);

    // Make sure it's not parallel
    if(glm::abs(glm::dot(_upVector, other)) > 0.99f)
    {
        other = glm::vec3(1,0,0);
    }

    glm::vec3 basisZ = glm::normalize(glm::cross(_upVector, other));
    glm::vec3 basisX = glm::cross(_upVector, basisZ);

    return glm::mat3x3(basisX, _upVector, basisZ);
}

glm::vec3 MathsHelpers::GenerateNormalFromPitchYaw(f32 _pitch, f32 _yaw)
{
    glm::vec4 unitVector = glm::vec4(1.f,0.f,0.f, 1.f);

    unitVector = glm::rotate(glm::mat4(1.f), _pitch, glm::vec3(0.f,0.f,1.f)) * unitVector;
    unitVector = glm::rotate(glm::mat4(1.f), _yaw, glm::vec3(0.f,1.f,0.f)) * unitVector;

    return glm::vec3(unitVector);
}

glm::vec3 MathsHelpers::GenerateArbitraryNormal(glm::vec3 _tangent)
{
    // Pick an arbitrary vector
    glm::vec3 other = glm::vec3(0,1,0);

    // Make sure it's not parallel
    if(glm::abs(glm::dot(_tangent, other)) > 0.99f)
    {
        other = glm::vec3(1,0,0);
    }

    const glm::vec3 normal = glm::normalize(glm::cross(_tangent, other));

    return normal;
}

