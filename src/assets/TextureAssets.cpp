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
    case TextureAsset_Branch_01_01:
        return "branches/Branch_01_01.png";
        
    case TextureAsset_Billboard_Grass:
        return "Billboard_Grass.png";
    case TextureAsset_Billboard_Leaves:
        return "Billboard_Leaves.png";
    case TextureAsset_Billboard_DirtParticle:
        return "Billboard_DirtParticle.png";

    case TextureAsset_Volume_Perlin128:
        return "volumes/Volume_Perlin128.png";
    case TextureAsset_Volume_Grass128:
        return "volumes/Volume_Grass128.png";
        
    case TextureAsset_Icon_Topsoil512:
        return "ui/Icon_Topsoil_512.png";
    case TextureAsset_Icon_Dirt512:
        return "ui/Icon_Dirt_512.png";
    case TextureAsset_Icon_Stone512:
        return "ui/Icon_Stone_512.png";
    case TextureAsset_Icon_Sand512:
        return "ui/Icon_Sand_512.png";
        
    case TextureAsset_Invalid:
        default:
            return "INVALID";
    }
    static_assert(static_cast<TextureAssetID>(TextureAsset_Count) == 14);
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
        case TextureAsset_Branch_01_01:
            [[fallthrough]];
        case TextureAsset_Billboard_Grass:
            [[fallthrough]];
        case TextureAsset_Billboard_Leaves:
            [[fallthrough]];
        case TextureAsset_Billboard_DirtParticle:
            [[fallthrough]];
        case TextureAsset_Icon_Topsoil512:
            [[fallthrough]];
        case TextureAsset_Icon_Dirt512:
            [[fallthrough]];
        case TextureAsset_Icon_Stone512:
            [[fallthrough]];
        case TextureAsset_Icon_Sand512:
            [[fallthrough]];
            
        case TextureAsset_Invalid:
        default:
            return TextureAssetType::Image;

    }
    static_assert(static_cast<TextureAssetID>(TextureAsset_Count) == 14);
}
