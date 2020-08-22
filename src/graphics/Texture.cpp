//
// Created by alastair on 30/07/17.
//

#include "Texture.h"

#include <SFML/Graphics/Image.hpp>

#include <src/graphics/GLHelpers.h>
#include <src/graphics/ShaderProgram.h>

void Texture::AcquireResources(TextureAssetID _asset)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    m_TextureType = Assets::GetTextureAssetType(_asset);

    switch (m_TextureType)
    {
        case TextureAssetType::Image:
            CreateImageTexture(Assets::GetTextureAssetName(_asset));
            break;
        case TextureAssetType::Cubemap:
            CreateCubemapTexture(Assets::GetTextureAssetName(_asset));
            break;
        case TextureAssetType::Volume:
            CreateVolumeTexture(Assets::GetTextureAssetName(_asset));
            break;
        default:
            break;
    }
    static_assert(static_cast<u32>(TextureAssetType::Count) == 3);
}

void Texture::ReleaseResources()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    glDeleteTextures(1, &m_TextureHandle);
    GLHelpers::ReportError("glDeleteTextures");
}

void Texture::CreateImageTexture(std::string _fileName)
{
    sf::Image imgData;
    const std::string path = Globals::FREEPLANET_ASSET_PATH + "textures/" + _fileName;
    const bool texLoadSuccess = imgData.loadFromFile(path);

    if(!texLoadSuccess)
    {
        LogError("Failed to load a texture from path: " + path);
    }

    imgData.flipVertically();

    glGenTextures(1, &m_TextureHandle);
    GLHelpers::ReportError("glGenTextures");

    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
    GLHelpers::ReportError("glBindTexture");

    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA,
            imgData.getSize().x, imgData.getSize().y,
            0,
            GL_RGBA, GL_UNSIGNED_BYTE, imgData.getPixelsPtr()
    );
    GLHelpers::ReportError("glTexImage2D");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLHelpers::ReportError("glTexParameter setup");
}

void Texture::CreateVolumeTexture(std::string _fileName)
{
    sf::Image imgData;
    const std::string path = Globals::FREEPLANET_ASSET_PATH + "textures/" + _fileName;
    const bool texLoadSuccess = imgData.loadFromFile(path);

    if(!texLoadSuccess)
    {
        LogError("Failed to load a texture from path: " + path);
    }

    imgData.flipVertically();

    glGenTextures(1, &m_TextureHandle);
    GLHelpers::ReportError("glGenTextures");

    glBindTexture(GL_TEXTURE_3D, m_TextureHandle);
    GLHelpers::ReportError("glBindTexture");

    assert(imgData.getSize().x * imgData.getSize().x == imgData.getSize().y);

    glTexImage3D(
            GL_TEXTURE_3D, 0, GL_RGBA,
            imgData.getSize().x, imgData.getSize().x, imgData.getSize().x,
            0,
            GL_RGBA, GL_UNSIGNED_BYTE, imgData.getPixelsPtr()
    );
    GLHelpers::ReportError("glTexImage3D");

    glGenerateTextureMipmap(m_TextureHandle);

    // TODO Enable or disable mipmaps per texture.
     glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // TODO How does this map into 3D?
    GLHelpers::ReportError("glTexParameter setup");
}

void Texture::Bind(ShaderProgram *_program, const std::string& _name, u32 _texUnitIdx)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    switch (m_TextureType)
    {
    case TextureAssetType::Image:
        BindAsTexture(_program, _name, _texUnitIdx);
        break;
    case TextureAssetType::Cubemap:
        BindAsCubemap(_program, _name, _texUnitIdx);
        break;
    case TextureAssetType::Volume:
        BindAsVolume(_program, _name, _texUnitIdx);
        break;
    case TextureAssetType::Count:
        break;
    }
    static_assert(static_cast<u32>(TextureAssetType::Count) == 3);
}

void Texture::BindAsTexture(ShaderProgram* _program, const std::string& _name, u32 _texUnitIdx)
{
    glActiveTexture(GL_TEXTURE0 + _texUnitIdx);
    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
    const GLuint sampler = glGetUniformLocation(_program->GetProgramHandle(), _name.c_str());
    GLHelpers::ReportError("glGetUniformLocation");
    glUniform1i(sampler, _texUnitIdx);
    GLHelpers::ReportError("glUniform1i");
}

void Texture::BindAsCubemap(ShaderProgram* _program, const std::string& _name, u32 _texUnitIdx)
{
    glActiveTexture(GL_TEXTURE0 + _texUnitIdx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureHandle);
    const GLuint sampler = glGetUniformLocation(_program->GetProgramHandle(), _name.c_str());
    GLHelpers::ReportError("glGetUniformLocation");
    glUniform1i(sampler, _texUnitIdx);
    GLHelpers::ReportError("glUniform1i");
}

void Texture::BindAsVolume(ShaderProgram* _program, const std::string& _name, u32 _texUnitIdx)
{
    glActiveTexture(GL_TEXTURE0 + _texUnitIdx);
    glBindTexture(GL_TEXTURE_3D, m_TextureHandle);
    const GLuint sampler = glGetUniformLocation(_program->GetProgramHandle(), _name.c_str());
    GLHelpers::ReportError("glGetUniformLocation");
    glUniform1i(sampler, _texUnitIdx);
    GLHelpers::ReportError("glUniform1i");
}

void Texture::Unbind(u32 _texUnitIdx)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    glActiveTexture(GL_TEXTURE0 + _texUnitIdx);
    if (m_TextureType == TextureAssetType::Volume)
    {
        glBindTexture(GL_TEXTURE_3D, 0);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::CreateCubemapTexture(std::string _directory)
{
    const std::string path = Globals::FREEPLANET_ASSET_PATH + "textures/" + _directory;

    glGenTextures(1, &m_TextureHandle);
    GLHelpers::ReportError("glGenTextures");

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureHandle);
    GLHelpers::ReportError("glBindTexture");

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLHelpers::ReportError("glTexParameter setup");

    for (int faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
        sf::Image imgData;
        const bool texLoadSuccess = imgData.loadFromFile(path + Assets::CUBEMAP_PATHS[faceIdx]);

        GLuint faceEnum;
        switch (faceIdx)
        {
            case 0:
                faceEnum = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                break;
            case 1:
                faceEnum = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
                break;
            case 2:
                faceEnum = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
                break;
            case 3:
                faceEnum = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
                break;
            case 4:
                faceEnum = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
                break;
            case 5:
                faceEnum = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
                break;
        }

        if (!texLoadSuccess)
        {
            LogError("Failed to load a cubemap face texture from path: " + path + Assets::CUBEMAP_PATHS[faceIdx]);
        }

        imgData.flipVertically();

        glTexImage2D(
                faceEnum, 0, GL_RGBA,
                imgData.getSize().x, imgData.getSize().y,
                0,
                GL_RGBA, GL_UNSIGNED_BYTE, imgData.getPixelsPtr()
        );

        GLHelpers::ReportError("glTexImage2D");
    }
}
