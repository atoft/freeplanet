//
// Created by alastair on 30/07/17.
//

#pragma once

#include <vector>

#include <src/assets/TextureAssets.h>
#include <src/graphics/GLHelpers.h>
#include <src/tools/globals.h>

class ShaderProgram;

class Texture
{
public:
    Texture(TextureAssetID _asset);


    void BindAsTexture(ShaderProgram *program, int textureNumber);
    void Unbind();

private:
    void CreateImageTexture(std::string _fileName);
    void CreateCubemapTexture(std::string _directory);


private:
    GLuint m_TextureHandle;
    TextureAssetType m_TextureType;
public:
    GLuint GetTextureHandle() const;

    TextureAssetType GetTextureType() { return m_TextureType; };

    void BindAsCubemap(ShaderProgram* _program, int _textureNumber);
};
