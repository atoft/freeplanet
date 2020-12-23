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
    ShaderAsset_Lit_Inst_Textured,
    ShaderAsset_Lit_Inst_AlphaTest_NormalUp,
    ShaderAsset_Lit_Inst_AlphaBlend_NormalUp,
    ShaderAsset_Skybox,

    ShaderAsset_Terrain_Base,

    ShaderAsset_Count
};
namespace Assets
{
    std::string GetShaderAssetName(ShaderAssetID _assetID);
}
