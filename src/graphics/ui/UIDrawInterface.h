//
// Created by alastair on 01/10/18.
//

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

private:
    UIDisplay* m_UIDisplay = nullptr;
};
