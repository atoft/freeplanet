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
#include <src/tools/globals.h>

using TextureAssetID = u32;
enum TextureAsset
{
    TextureAsset_Invalid = 0,
    TextureAsset_Dev_512,
    TextureAsset_Cubemap_Dev,
    
    TextureAsset_Branch_01_01,
    TextureAsset_Campfire,

    
    TextureAsset_Billboard_Grass,
    TextureAsset_Billboard_Leaves,
    TextureAsset_Billboard_DirtParticle,
    TextureAsset_Billboard_Flame,
    TextureAsset_Billboard_Smoke,
    TextureAsset_Volume_Perlin128,
    TextureAsset_Volume_Grass128,

    TextureAsset_Icon_Topsoil512,
    TextureAsset_Icon_Dirt512,
    TextureAsset_Icon_Stone512,
    TextureAsset_Icon_Sand512,
    TextureAsset_Icon_Fire512,
    
    TextureAsset_Count
};

enum class TextureAssetType
{
    Image = 0,
    Cubemap,
    Volume,
    Count
};

namespace Assets
{
    std::string GetTextureAssetName(TextureAssetID _assetID);
    TextureAssetType GetTextureAssetType(TextureAssetID _assetID);

    const std::vector<std::string> CUBEMAP_PATHS =
            {
                    "posx.png",
                    "negx.png",
                    "posy.png",
                    "negy.png",
                    "posz.png",
                    "negz.png",
            };
};
