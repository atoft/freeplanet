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

#include "ShaderAssets.h"

std::string Assets::GetShaderAssetName(ShaderAssetID _assetID)
{
    assert(_assetID < static_cast<ShaderAssetID>(ShaderAsset_Count));

    switch(static_cast<ShaderAsset >(_assetID))
    {
        case ShaderAsset_Default:
            return "Default";
        case ShaderAsset_Unlit_ShowNormals:
            return "Unlit_ShowNormals";
        case ShaderAsset_Unlit_Untextured:
            return "Unlit_Untextured";
        case ShaderAsset_Lit_Textured:
            return "Lit_Textured";
        case ShaderAsset_Lit_Inst_Textured:
            return "Lit_Inst_Textured";
        case ShaderAsset_Lit_AlphaTest_NormalUp:
            return "Lit_AlphaTest_NormalUp";
        case ShaderAsset_Lit_Inst_AlphaTest_NormalUp:
            return "Lit_Inst_AlphaTest_NormalUp";
        case ShaderAsset_Lit_Inst_AlphaBlend_NormalUp:
            return "Lit_Inst_AlphaBlend_NormalUp";
        case ShaderAsset_Skybox:
            return "Skybox";
        case ShaderAsset_Terrain_Base:
            return "Terrain_Base";
        case ShaderAsset_Invalid:
        default:
            return "INVALID";
    }
    static_assert(static_cast<ShaderAssetID>(ShaderAsset_Count) == 11);
}
