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

#include <src/graphics/DynamicMesh.h>

// This is the main thread representation of a dynamic mesh. Meshes can be pushed into this by gameplay code.
// The RenderHandler can then make the needed requests to the Render thread to update and draw the mesh.
class DynamicMeshHandle
{
    friend class RenderHandler;

public:
    void RequestMeshUpdate(const RawMesh& _mesh);
    void RequestDestruction();

    // Returns true if the latest version of the DynamicMesh has been loaded on the GPU.
    bool IsUpToDate() const { return m_DynamicMeshId != DYNAMICMESHID_INVALID; };

    const RawMesh& GetRawMesh() const { return m_RawMesh; };
    DynamicMeshID GetID() const { return m_DynamicMeshId; };
    
private:
    DynamicMeshID m_DynamicMeshId = DYNAMICMESHID_INVALID;
    DynamicMeshID m_PreviousDynamicMeshId = DYNAMICMESHID_INVALID;

    // This is the CPU data source of the dynamic mesh that will be on the CPU.
    RawMesh m_RawMesh;
};
