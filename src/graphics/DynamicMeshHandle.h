//
// Created by alastair on 28/09/19.
//

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

private:
    DynamicMeshID m_DynamicMeshId = DYNAMICMESHID_INVALID;
    DynamicMeshID m_PreviousDynamicMeshId = DYNAMICMESHID_INVALID;

    // This is the CPU data source of the dynamic mesh that will be on the CPU.
    RawMesh m_RawMesh;
};