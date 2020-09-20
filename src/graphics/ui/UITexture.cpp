//
// Created by alastair on 19/09/2020.
//

#include <vcpkg/installed/x64-linux/include/SFML/Graphics/Sprite.hpp>
#include "UITexture.h"

void UITexture::AcquireResources(TextureAssetID _asset)
{
    LogMessage("ACQUIRE!!!!");
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    const std::string assetName = Assets::GetTextureAssetName(_asset);

    const std::string path = Globals::FREEPLANET_ASSET_PATH + "textures/" + assetName;

    m_Texture = std::make_shared<sf::Texture>();
    const bool texLoadSuccess = m_Texture->loadFromFile(path);

    if(!texLoadSuccess)
    {
        LogError("Failed to load a texture from path: " + path);
    }
}

void UITexture::ReleaseResources()
{
    m_Texture = nullptr;
    LogMessage("RELEASE!!!");
}
