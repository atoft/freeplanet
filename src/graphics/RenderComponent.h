//
// Created by alastair on 25/03/17.
//

#pragma once

#include <vector>

#include <src/engine/AssetHandle.h>
#include <src/graphics/ShaderProgram.h>
#include <src/graphics/StaticMesh.h>
#include <src/graphics/Texture.h>
#include <src/world/WorldObjectComponent.h>

struct Triangle;

class RenderComponent : public WorldObjectComponent
{
public:
    RenderComponent(World* _world, WorldObjectID _ownerID, AssetHandle<StaticMesh> _mesh, AssetHandle<ShaderProgram> _shader, AssetHandle<Texture> _texture
                    );

    AssetHandle<ShaderProgram> GetShader() const { return m_Shader; };
    AssetHandle<Texture> GetTexture() const { return m_Texture; };
    AssetHandle<StaticMesh> GetMesh() const { return m_Mesh; };

protected:
    AssetHandle<StaticMesh> m_Mesh;
    AssetHandle<ShaderProgram> m_Shader;
    AssetHandle<Texture> m_Texture;

};

