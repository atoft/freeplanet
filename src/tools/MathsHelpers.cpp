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
glm::mat4x4 MathsHelpers::GenerateRotationMatrixFromNormal(glm::vec3 normal)
{
    // Pick an arbitrary vector
    glm::vec3 other = glm::vec3(1,0,0);

    // Make sure it's not parallel
    if(glm::abs(glm::dot(normal, other)) > 0.99f)
    {
        other = glm::vec3(0,1,0);
    }

    glm::vec3 basisY = glm::normalize(glm::cross(normal, other));
    glm::vec3 basisZ = glm::cross(normal, basisY);

    glm::mat4x4 matrix = glm::mat3x3(normal, basisY, basisZ);
    matrix[3][3] = 1.f;

    return matrix;
}
