//
// Created by alastair on 23/09/18.
//

#pragma once

#include <string>

#include <src/tools/globals.h>

using ShaderAssetID = u32;
enum ShaderAsset
{
    ShaderAsset_Invalid = 0,
    ShaderAsset_Default,
    ShaderAsset_Unlit_ShowNormals,
    ShaderAsset_Unlit_Untextured,
    ShaderAsset_Lit_Textured,
    ShaderAsset_Lit_AlphaTest_NormalUp,
    ShaderAsset_Skybox,

    ShaderAsset_Terrain_Base,

    ShaderAsset_Count
};
namespace Assets
{
    std::string GetShaderAssetName(ShaderAssetID _assetID);
}
