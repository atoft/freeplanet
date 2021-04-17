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

#include <glm/vec4.hpp>
#include <SFML/System/Vector2.hpp>

#include <src/engine/AssetHandle.h>
#include <src/graphics/Color.h>
#include <src/tools/globals.h>

struct AABB;
class UIDisplay;

enum class FontStyle
{
    Sans = 0,
    Monospace,
    Count
};

enum class UIAnchorPosition
{
    Centered = 0,
    TopLeft,
    BottomLeft
};

class UITexture;

class UIDrawInterface
{
public:
    void SetUIDisplay(UIDisplay* _display) { m_UIDisplay = _display;};
    void DrawString(glm::vec2 _position, std::string _text, f32 _scale, Color _color = Color(1.f), FontStyle _style = FontStyle::Sans, UIAnchorPosition _anchor = UIAnchorPosition::Centered, bool _exactPixels = false, s32 _caretPos = -1);
    void DrawRectangle(glm::vec2 _position, glm::vec2 _scale, Color _color = Color(1.f), UIAnchorPosition _anchor = UIAnchorPosition::Centered);
    void DrawSprite(glm::vec2 _position, glm::vec2 _scale, AssetHandle<UITexture> _texture, Color _color = Color(1.f), UIAnchorPosition _anchor = UIAnchorPosition::Centered);
    void FillScreen(Color _color);

    void DebugDrawSphere(glm::ivec3 zoneCoordinates, glm::vec3 _position, f32 _radius, Color _color = Color(0.f, 0.f, 0.f, 1.f));
    void DebugDrawArrow(glm::ivec3 zoneCoordinates, glm::vec3 _position, f32 _length, glm::vec3 _normal, Color _color = Color(0.f, 0.f, 0.f, 1.f));
    void DebugDrawAABB(glm::ivec3 zoneCoordinates, const glm::vec3& _position, const AABB& _aabb);

    glm::ivec2 GetDisplayResolution() const;

    bool IsInAABB(glm::uvec2 _point, glm::uvec2 _boxPosition, glm::uvec2 _boxScale, UIAnchorPosition _anchor) const;
    bool IsUsingMouse() const;

private:
    UIDisplay* m_UIDisplay = nullptr;
};
