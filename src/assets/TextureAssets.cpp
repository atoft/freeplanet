//
// Created by alastair on 23/09/18.
//

#include "TextureAssets.h"

std::string Assets::GetTextureAssetName(TextureAssetID _assetID)
{
    assert(_assetID < static_cast<TextureAssetID>(TextureAsset_Count));

    switch(static_cast<TextureAsset>(_assetID))
    {
        case TextureAsset_Dev_512:
            return "Dev_512.png";
        case TextureAsset_Cubemap_Dev:
            return "cubemaps/dev/";
        case TextureAsset_Tree:
            return "Tree.jpg";
        case TextureAsset_Billboard_Grass:
            return "Billboard_Grass.png";
        case TextureAsset_Volume_Perlin128:
            return "volumes/Volume_Perlin128.png";
        case TextureAsset_Volume_Grass128:
            return "volumes/Volume_Grass128.png";
        case TextureAsset_Invalid:
        default:
            return "INVALID";
    }
    static_assert(static_cast<TextureAssetID>(TextureAsset_Count) == 7);
}

TextureAssetType Assets::GetTextureAssetType(TextureAssetID _assetID)
{
    assert(_assetID < static_cast<TextureAssetID>(TextureAsset_Count));

    switch(static_cast<TextureAsset>(_assetID))
    {
        case TextureAsset_Cubemap_Dev:
            return TextureAssetType::Cubemap;
        case TextureAsset_Volume_Perlin128:
            [[fallthrough]];
        case TextureAsset_Volume_Grass128:
            return TextureAssetType::Volume;
        case TextureAsset_Dev_512:
            [[fallthrough]];
        case TextureAsset_Tree:
            [[fallthrough]];
        case TextureAsset_Billboard_Grass:
            [[fallthrough]];
        case TextureAsset_Invalid:
        default:
            return TextureAssetType::Image;

    }
    static_assert(static_cast<TextureAssetID>(TextureAsset_Count) == 7);
}