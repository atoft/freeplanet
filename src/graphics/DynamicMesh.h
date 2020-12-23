/*
 * Copyright 2017-2020 Alastair Toft
 *
 * This file is part of freeplanet.
 *
 * freeplanet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeplanet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeplanet. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <src/graphics/DynamicMeshID.h>
#include <src/graphics/RawMesh.h>
#include <src/graphics/Scene.h>

class ShaderProgram;
struct TerrainChunk;

class DynamicMesh
{
public:
    DynamicMesh() = default;
    ~DynamicMesh() = default;

    const Renderable::Mesh& GetMesh() const { return m_Mesh; };

    void LoadToGPU(const RawMesh& _mesh);
    void ReleaseFromGPU();

private:
    Renderable::Mesh m_Mesh;
};
