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

#include "UITexture.h"

void UITexture::AcquireResources(TextureAssetID _asset)
{
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
}
