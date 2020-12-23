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

#include <vector>

#include <src/engine/AssetHandle.h>
#include <src/graphics/DynamicMeshID.h>
#include <src/graphics/ShaderProgram.h>
#include <src/graphics/StaticMesh.h>
#include <src/graphics/Texture.h>
#include <src/world/WorldObjectComponent.h>

struct Triangle;

class RenderComponent : public WorldObjectComponent
{
public:
    RenderComponent(World* _world, WorldObjectID _ownerID, AssetHandle<StaticMesh> _mesh, AssetHandle<ShaderProgram> _shader, AssetHandle<Texture> _texture, MeshType _meshType, DynamicMeshID _dynamicID = DYNAMICMESHID_INVALID
                    );

    AssetHandle<ShaderProgram> GetShader() const { return m_Shader; };
    AssetHandle<Texture> GetTexture() const { return m_Texture; };
    AssetHandle<StaticMesh> GetMesh() const { return m_Mesh; };
    MeshType GetMeshType() const { return m_MeshType; };

    // TODO @Memory Probably a separate component for dynamic meshes.
    mutable DynamicMeshID m_DynamicID = DYNAMICMESHID_INVALID;

    bool m_CanInstance = false;
    
protected:
    AssetHandle<StaticMesh> m_Mesh;
    AssetHandle<ShaderProgram> m_Shader;
    AssetHandle<Texture> m_Texture;
    MeshType m_MeshType;
};

