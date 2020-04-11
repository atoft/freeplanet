//
// Created by alastair on 25/03/17.
//

#include "RenderComponent.h"

#include <vector>
#include <src/tools/globals.h>

RenderComponent::RenderComponent(
    World* _world,
    WorldObjectID _ownerID,
    AssetHandle<StaticMesh> _mesh,
    AssetHandle<ShaderProgram> _shader,
    AssetHandle<Texture> _texture)
{
    m_World = _world;
    m_WorldObjectID = _ownerID;

    m_Shader =_shader;
    m_Texture = _texture;

    m_Mesh = _mesh;
}
