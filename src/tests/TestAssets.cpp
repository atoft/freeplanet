//
// Created by alastair on 16/08/2020.
//

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
