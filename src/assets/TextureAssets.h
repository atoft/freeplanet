//
// Created by alastair on 23/09/18.
//

#pragma once

#include <vector>
#include <src/tools/globals.h>

using TextureAssetID = u32;
enum TextureAsset
{
    TextureAsset_Invalid = 0,
    TextureAsset_Dev_512,
    TextureAsset_Cubemap_Dev,
    
    TextureAsset_Tree,
    TextureAsset_Branch_01_01,
    
    TextureAsset_Billboard_Grass,
    TextureAsset_Billboard_Leaves,
    TextureAsset_Billboard_DirtParticle,
    TextureAsset_Volume_Perlin128,
    TextureAsset_Volume_Grass128,

    TextureAsset_Icon_Topsoil512,
    TextureAsset_Icon_Dirt512,
    TextureAsset_Icon_Stone512,
    TextureAsset_Icon_Sand512,
    
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
