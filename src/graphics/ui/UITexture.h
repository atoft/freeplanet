/*
 * Copyright 2020 Alastair Toft
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <SFML/Graphics/Texture.hpp>

#include <src/assets/TextureAssets.h>
#include <src/engine/AssetHandle.h>
#include <src/graphics/Color.h>

// Allows for loading textures as an asset for SFML.
class UITexture
{
public:
    void AcquireResources(TextureAssetID _asset);
    void ReleaseResources();

    std::shared_ptr<sf::Texture> m_Texture;
};

// Use as a drawable for the SFML UI.
// This is needed to allow the sfml asset to be
struct UISprite
{
    glm::vec2 m_Origin = glm::vec2(0.f);
    f32 m_Scale = 1.f;
    Color m_Color = Color(1.f);
    AssetHandle<UITexture> m_UITexture;
};