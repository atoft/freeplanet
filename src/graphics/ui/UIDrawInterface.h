//
// Created by alastair on 01/10/18.
//

#pragma once

#include <glm/vec4.hpp>
#include <SFML/System/Vector2.hpp>

#include <src/tools/globals.h>

using Color = glm::vec4;

class UIDisplay;

enum class FontStyle
{
    Sans = 0,
    Monospace,
    Count
};

class UIDrawInterface
{
public:
    void SetUIDisplay(UIDisplay* _display) { m_UIDisplay = _display;};
    void DrawString(glm::vec2 _position, std::string _text, f32 _scale, Color _color = Color(1.f), FontStyle _style = FontStyle::Sans, bool _exactPixels = false, s32 _caretPos = -1);
    void DrawRectangle(glm::vec2 _position, glm::vec2 _scale, Color _color = Color(1.f));
    void DrawSpriteFromDisk(glm::vec2 _position, glm::vec2 _scale, std::string _path);

    void DebugDrawSphere(glm::ivec3 zoneCoordinates, glm::vec3 _position, f32 _radius);
    void DebugDrawArrow(glm::ivec3 zoneCoordinates, glm::vec3 _position, f32 _length, glm::vec3 _normal);

    glm::ivec2 GetDisplayResolution() const;

private:
    glm::uvec2 GetScaledVector(u32 x, u32 y) const;
    sf::Vector2f GetSFMLScaledVector(u32 x, u32 y) const;

private:
    UIDisplay* m_UIDisplay;
};
