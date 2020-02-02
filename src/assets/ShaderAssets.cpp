//
// Created by alastair on 23/09/18.
//

#include "ShaderAssets.h"

std::string Assets::GetShaderAssetName(ShaderAssetID _assetID)
{
    assert(_assetID < static_cast<ShaderAssetID>(ShaderAsset::Count));

    switch(static_cast<ShaderAsset >(_assetID))
    {
        case ShaderAsset_Default:
            return "Default";
        case ShaderAsset_Unlit_ShowNormals:
            return "Unlit_ShowNormals";
        case ShaderAsset_Unlit_Untextured:
            return "Unlit_Untextured";
        case ShaderAsset_Skybox:
            return "Skybox";
        case ShaderAsset_Terrain_Base:
            return "Terrain_Base";
        case ShaderAsset_Invalid:
        default:
            return "INVALID";
    }
    static_assert(static_cast<ShaderAssetID>(ShaderAsset::Count) == 6);
}
