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

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <src/graphics/ui/UITexture.h>
#include <vcpkg/installed/x64-linux/include/SFML/Graphics/Sprite.hpp>
#include "UIDisplay.h"

class SFMLDraw
{
public:
    static void Draw(const sf::Drawable& _drawable, std::shared_ptr<sf::RenderWindow> _window)
    {
        _window->draw(_drawable);
    }

    static void Draw(const UISprite& _sprite, std::shared_ptr<sf::RenderWindow> _window)
    {
        UITexture* uiTexture = _sprite.m_UITexture.GetAsset();

        if (uiTexture == nullptr)
        {
            return;
        }

        const sf::Vector2u size = uiTexture->m_Texture->getSize();

        sf::Sprite sprite;
        sprite.setTexture(*uiTexture->m_Texture);
        sprite.setPosition(_sprite.m_Origin.x, _sprite.m_Origin.y);
        sprite.setScale(_sprite.m_Scale / size.x, _sprite.m_Scale / size.y);

        sprite.setColor(sf::Color(
                _sprite.m_Color.r * 255,
                _sprite.m_Color.g * 255,
                _sprite.m_Color.b * 255,
                _sprite.m_Color.a * 255));

        _window->draw(sprite);
    }
};