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
    Texture() = default;

    void AcquireResources(TextureAssetID _asset);
    void ReleaseResources();

    void Bind(ShaderProgram* _program, u32 _textureNumber);
    void Unbind();

    TextureAssetType GetTextureType() { return m_TextureType; };



private:
    void CreateImageTexture(std::string _fileName);
    void CreateVolumeTexture(std::string _fileName);
    void CreateCubemapTexture(std::string _directory);

    void BindAsTexture(ShaderProgram* _program, u32 _textureNumber);
    void BindAsCubemap(ShaderProgram* _program, u32 _textureNumber);
    void BindAsVolume(ShaderProgram* _program, u32 _textureNumber);

private:
    GLuint m_TextureHandle;
    TextureAssetType m_TextureType;
};
