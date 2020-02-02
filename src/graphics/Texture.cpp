//
// Created by alastair on 30/07/17.
//

#include "Texture.h"

#include <SFML/Graphics/Image.hpp>

#include <src/graphics/ShaderProgram.h>

Texture::Texture(TextureAssetID _asset)
{
    m_TextureType = Assets::GetTextureAssetType(_asset);

    switch (m_TextureType)
    {
        case TextureAssetType::Image:
            CreateImageTexture(Assets::GetTextureAssetName(_asset));
            break;
        case TextureAssetType::Cubemap:
            CreateCubemapTexture(Assets::GetTextureAssetName(_asset));
            break;
        default:
            break;
    }
    static_assert(static_cast<u32>(TextureAssetType::Count) == 2);
}

void Texture::CreateImageTexture(std::string _fileName)
{
    sf::Image imgData;
    std::string path = Globals::FREEPLANET_ASSET_PATH + "textures/" + _fileName;
    bool texLoadSuccess = imgData.loadFromFile(path);

    if(!texLoadSuccess)
    {
        LogError("Failed to load a texture from path: " + path);
    }

    imgData.flipVertically();

    glGenTextures(1, &m_TextureHandle);
    Globals::ReportGLError("glGenTextures");

    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
    Globals::ReportGLError("glBindTexture");

    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA,
            imgData.getSize().x, imgData.getSize().y,
            0,
            GL_RGBA, GL_UNSIGNED_BYTE, imgData.getPixelsPtr()
    );
    Globals::ReportGLError("glTexImage2D");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    Globals::ReportGLError("glTexParameter setup");
}

void Texture::BindAsTexture(ShaderProgram *program, int textureNumber)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
    GLuint sampler = glGetUniformLocation(program->GetProgramHandle(), "tex2D_0");
    Globals::ReportGLError("glGetUniformLocation");
    glUniform1i(sampler, 0);
    Globals::ReportGLError("glUniform1i");

}

void Texture::BindAsCubemap(ShaderProgram* _program, int _textureNumber)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureHandle);
    GLuint sampler = glGetUniformLocation(_program->GetProgramHandle(), "texCUBE_0");
    Globals::ReportGLError("glGetUniformLocation");
    glUniform1i(sampler, 0);
    Globals::ReportGLError("glUniform1i");
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::GetTextureHandle() const
{
    return m_TextureHandle;
}

void Texture::CreateCubemapTexture(std::string _directory)
{
    const std::string path = Globals::FREEPLANET_ASSET_PATH + "textures/" + _directory;

    glGenTextures(1, &m_TextureHandle);
    Globals::ReportGLError("glGenTextures");

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureHandle);
    Globals::ReportGLError("glBindTexture");

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    Globals::ReportGLError("glTexParameter setup");

    for (int faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
        sf::Image imgData;
        bool texLoadSuccess = imgData.loadFromFile(path + Assets::CUBEMAP_PATHS[faceIdx]);

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

        Globals::ReportGLError("glTexImage2D");
    }
}
