//
// Created by alastair on 16/11/18.
//

#pragma once

#include <array>
#include <vector>

#include <src/graphics/Color.h>
#include <src/tools/globals.h>
#include <src/world/terrain/TerrainSubstance.h>

struct Triangle;

using RawTriangle = std::array<u32, 3>;

struct RawMesh
{
    bool IsEmpty() const { return  m_Vertices.empty(); };

    void Translate(const glm::vec3& _offset);
    void Transform(const glm::mat4& _transform);
    
    // Joins two meshes together. Does not merge identical verts.
    void Append(const RawMesh& _other);

    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_TextureCoordinates;
    std::vector<Color> m_Colors;
    std::vector<TerrainSubstance> m_TerrainSubstance;
    static constexpr u32 ms_VertexTypesCount = 5;

    std::vector<RawTriangle> m_Faces;
};
