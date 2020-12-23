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

#include "RenderComponent.h"

#include <vector>
#include <src/tools/globals.h>

RenderComponent::RenderComponent(
    World* _world,
    WorldObjectID _ownerID,
    AssetHandle<StaticMesh> _mesh,
    AssetHandle<ShaderProgram> _shader,
    AssetHandle<Texture> _texture,
    MeshType _meshType,
    DynamicMeshID _dynamicID)
{
    m_World = _world;
    m_WorldObjectID = _ownerID;

    m_Shader =_shader;
    m_Texture = _texture;

    m_Mesh = _mesh;
    m_MeshType = _meshType;

    m_DynamicID = _dynamicID;
}
