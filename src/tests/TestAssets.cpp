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

#include "TestAssets.h"

#include <SFML/Graphics/Image.hpp>

#include <src/assets/TextureAssets.h>
#include <src/assets/MeshAssets.h>
#include <src/graphics/MeshImport.h>
#include <src/assets/ShaderAssets.h>
#include <src/tests/TestHelpers.h>
#include <src/graphics/ShaderProgram.h>

bool Test::TestAssets()
{
    bool result = true;

    result &= ValidateTextures();
    result &= ValidateStaticMeshes();
    result &= ValidateShaders();

    return result;
}

bool Test::ValidateTextures()
{
    bool success = true;

    for (TextureAssetID id = TextureAsset_Invalid + 1; id < TextureAsset_Count; ++id)
    {
        if (Assets::GetTextureAssetType(id) == TextureAssetType::Cubemap)
        {
            // Not validated for now.
            continue;
        }

        sf::Image imgData;
        const std::string path = Globals::FREEPLANET_ASSET_PATH + "textures/" + Assets::GetTextureAssetName(id);
        const bool texLoadSuccess = imgData.loadFromFile(path);

        if (!texLoadSuccess)
        {
            success = false;
            LogError("Failed to load " + path);
            continue;
        }

        if (Assets::GetTextureAssetType(id) == TextureAssetType::Volume)
        {
            const bool isCubeVolume = imgData.getSize().x * imgData.getSize().x == imgData.getSize().y;

            if (!isCubeVolume)
            {
                success = false;
                LogError(path + " was not the correct resolution for a cube volume.");
                continue;
            }
        }
    }

    return TestResult(success);
}

bool Test::ValidateStaticMeshes()
{
    bool success = true;

    for (MeshAssetID id = MeshAsset_Invalid + 1; id < MeshAsset_Count; ++id)
    {
        std::string modelPath = Globals::FREEPLANET_ASSET_PATH + "models/" + Assets::GetMeshAssetName(id) + ".obj";

        std::optional<MeshImport::ImportedMeshData> mesh = MeshImport::ImportOBJ(modelPath);

        success &= (mesh != std::nullopt);
    }

    return TestResult(success);
}

bool Test::ValidateShaders()
{
    bool success = true;

    for (ShaderAssetID id = ShaderAsset_Invalid + 1; id < ShaderAsset_Count; ++id)
    {
        std::string shaderName = Assets::GetShaderAssetName(id);

        success &= ShaderProgram::ValidateShader(shaderName);
    }

    return TestResult(success);
}
