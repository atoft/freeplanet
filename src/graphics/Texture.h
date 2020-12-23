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

#include <src/assets/TextureAssets.h>
#include <src/graphics/GLHelpers.h>
#include <src/tools/globals.h>

class ShaderProgram;

class Texture
{
public:
    Texture() = default;

    void AcquireResources(TextureAssetID _asset);
    void ReleaseResources();

    void Bind(ShaderProgram* _program, const std::string& _name, u32 _texUnitIdx);
    void Unbind(u32 _texUnitIdx);

    TextureAssetType GetTextureType() { return m_TextureType; };



private:
    void CreateImageTexture(std::string _fileName);
    void CreateVolumeTexture(std::string _fileName);
    void CreateCubemapTexture(std::string _directory);

    void BindAsTexture(ShaderProgram* _program, const std::string& _name, u32 _texUnitIdx);
    void BindAsCubemap(ShaderProgram* _program, const std::string& _name, u32 _texUnitIdx);
    void BindAsVolume(ShaderProgram* _program, const std::string& _name, u32 _texUnitIdx);

private:
    GLuint m_TextureHandle;
    TextureAssetType m_TextureType;
};
