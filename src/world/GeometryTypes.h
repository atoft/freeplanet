#pragma once

#include <glm/vec3.hpp>
#include <array>

#include <src/tools/globals.h>

struct Triangle
{
    std::array<glm::vec3, 3> m_Vertices;
};

struct TerrainChunk
{
    std::array<Triangle, 5> m_Triangles;
    u32 m_Count = 0;
};

struct AABB
{
    glm::vec3 m_Dimensions;
    glm::vec3 m_PositionOffset;

    AABB()
            : m_Dimensions(glm::vec3()),
              m_PositionOffset(glm::vec3()) {};

    AABB(glm::vec3 _dimensions, glm::vec3 _positionOffset = glm::vec3())
            : m_Dimensions(_dimensions),
              m_PositionOffset(_positionOffset) {};
};
