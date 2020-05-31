//
// Created by alastair on 01/10/18.
//

#include "UIDrawInterface.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <src/graphics/ui/UIDisplay.h>
#include <src/world/World.h>
#include <src/tools/MathsHelpers.h>
#include <src/graphics/ui/menus/UIConstants.h>

glm::ivec2 UIDrawInterface::GetDisplayResolution() const
{
    return glm::ivec2(m_UIDisplay->m_Window->getSize().x, m_UIDisplay->m_Window->getSize().y);
}

template<typename Vector2DType>
Vector2DType GetScaledSize(const glm::ivec2& _displayResolution, u32 x, u32 y)
{
    const f32 xRatio = static_cast<f32>(UIConstants::UIResolution.x) / static_cast<f32>(_displayResolution.x);
    const f32 yRatio = static_cast<f32>(UIConstants::UIResolution.y) / static_cast<f32>(_displayResolution.y);

    const f32 scaleRatio = glm::max(xRatio, yRatio);

    Vector2DType result;
    result.x = static_cast<f32>(x) / scaleRatio;
    result.y = static_cast<f32>(y) / scaleRatio;
    return result;
}

template<typename Vector2DType>
Vector2DType GetScaledPosition(const glm::ivec2& _displayResolution, u32 x, u32 y, UIAnchorPosition _anchor)
{
    const f32 xRatio = static_cast<f32>(UIConstants::UIResolution.x) / static_cast<f32>(_displayResolution.x);
    const f32 yRatio = static_cast<f32>(UIConstants::UIResolution.y) / static_cast<f32>(_displayResolution.y);

    const f32 scaleRatio = glm::max(xRatio, yRatio);

    Vector2DType result;
    result.x = static_cast<f32>(x) / scaleRatio;
    result.y = static_cast<f32>(y) / scaleRatio;

    switch (_anchor)
    {
        case UIAnchorPosition::Centered:
        {
            result.x += static_cast<f32>(_displayResolution.x - static_cast<f32>(UIConstants::UIResolution.x) / scaleRatio) / 2.f;
            result.y += static_cast<f32>(_displayResolution.y - static_cast<f32>(UIConstants::UIResolution.y) / scaleRatio) / 2.f;
            break;
        }
        case UIAnchorPosition::TopLeft:
            // No corrections needed.
            break;
        case UIAnchorPosition::BottomLeft:
        {
            result.y += _displayResolution.y - static_cast<f32>(UIConstants::UIResolution.y) / scaleRatio;
        }
        default:
            break;
    }
    return result;
}

void UIDrawInterface::DrawString(
        glm::vec2 _position,
        std::string _text,
        f32 _scale,
        Color _color,
        FontStyle _style,
        UIAnchorPosition _anchor,
        bool _exactPixels,
        s32 _caretPos)
{
    assert(m_UIDisplay != nullptr);
    assert(m_UIDisplay->m_Window != nullptr);

    sf::String sfString(_text);

    sf::Text sfText;
    switch(_style)
    {
        case FontStyle::Sans:
        {
            sfText = sf::Text(sfString, m_UIDisplay->m_UIFont);
            break;
        }
        case FontStyle::Monospace:
        {
            sfText = sf::Text(sfString, m_UIDisplay->m_MonospaceFont);
            break;
        }
        default:
            break;
    }
    static_assert(static_cast<u32>(FontStyle::Count) == 2);

    // TODO Consider removing the exactPixels rendering. It doesn't play very nicely with
    // UI scaling and anchoring.

    const glm::ivec2 displayResolution = GetDisplayResolution();

    const f32 scale = _exactPixels ? _scale : GetScaledSize<glm::uvec2>(displayResolution, _scale, _scale).y;

    const glm::uvec2 scaledPosition = GetScaledPosition<glm::uvec2>(displayResolution, _position.x, _position.y, _anchor);

    sfText.setCharacterSize(scale);
    sfText.setPosition(
            _exactPixels ? _position.x : scaledPosition.x,
            _exactPixels ? _position.y : scaledPosition.y);

    sfText.setFillColor(
            sf::Color(
                    _color.r * UIDisplay::MAX_COLOR,
                    _color.g * UIDisplay::MAX_COLOR,
                    _color.b * UIDisplay::MAX_COLOR,
                    _color.a * UIDisplay::MAX_COLOR));

    m_UIDisplay->m_DrawingQueue.push_back(sfText);

    if(_caretPos != -1)
    {
        sf::RectangleShape sfRectangle(sf::Vector2f(scale, scale));

        sfRectangle.setFillColor(
                sf::Color(
                        0.9f * UIDisplay::MAX_COLOR,
                        0.9f * UIDisplay::MAX_COLOR,
                        0.9f * UIDisplay::MAX_COLOR,
                        0.9f * UIDisplay::MAX_COLOR));

        sfRectangle.setPosition(sfText.findCharacterPos(_caretPos).x,
                                _exactPixels ? _position.y : GetScaledPosition<glm::uvec2>(displayResolution, _position.x, _position.y, _anchor).y);

        m_UIDisplay->m_DrawingQueue.push_back(sfRectangle);
    }
}

void UIDrawInterface::DrawRectangle(glm::vec2 _position, glm::vec2 _scale, Color _color, UIAnchorPosition _anchor)
{
    assert(m_UIDisplay != nullptr);
    assert(m_UIDisplay->m_Window != nullptr);

    const glm::ivec2 displayResolution = GetDisplayResolution();

    sf::RectangleShape sfRectangle(GetScaledSize<sf::Vector2f>(displayResolution, _scale.x, _scale.y));

    sfRectangle.setFillColor(
            sf::Color(
                    _color.r * UIDisplay::MAX_COLOR,
                    _color.g * UIDisplay::MAX_COLOR,
                    _color.b * UIDisplay::MAX_COLOR,
                    _color.a * UIDisplay::MAX_COLOR));



    sfRectangle.setPosition(GetScaledPosition<sf::Vector2f>(displayResolution, _position.x, _position.y, _anchor));

    m_UIDisplay->m_DrawingQueue.push_back(sfRectangle);
}

void UIDrawInterface::FillScreen(Color _color)
{
    assert(m_UIDisplay != nullptr);
    assert(m_UIDisplay->m_Window != nullptr);

    sf::RectangleShape sfRectangle(static_cast<sf::Vector2f>(m_UIDisplay->m_Window->getSize()));

    sfRectangle.setFillColor(
            sf::Color(
                    _color.r * UIDisplay::MAX_COLOR,
                    _color.g * UIDisplay::MAX_COLOR,
                    _color.b * UIDisplay::MAX_COLOR,
                    _color.a * UIDisplay::MAX_COLOR));

    m_UIDisplay->m_DrawingQueue.push_back(sfRectangle);
}

void UIDrawInterface::DebugDrawSphere(glm::ivec3 zoneCoordinates, glm::vec3 _position, f32 _radius, Color _color)
{
    StaticMesh* sphere = AssetHandle<StaticMesh>(MeshAsset_UnitUVSphere).GetAsset();

    if(sphere != nullptr)
    {
        Renderable::SceneObject sceneObject;
        sceneObject.m_Mesh = sphere->GetMesh();
        sceneObject.m_Transform = glm::scale(glm::translate(glm::mat4x4(1.f), _position),  glm::vec3(_radius));
        sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);
        sceneObject.m_BaseColor = _color;

        m_UIDisplay->m_Debug3DDrawingQueue.emplace_back(zoneCoordinates, sceneObject);
    }
}

void UIDrawInterface::DebugDrawArrow(glm::ivec3 zoneCoordinates, glm::vec3 _position, f32 _length, glm::vec3 _normal, Color _color)
{
    StaticMesh* arrow = AssetHandle<StaticMesh>(MeshAsset_Arrow).GetAsset();

    if(arrow != nullptr)
    {
        Renderable::SceneObject sceneObject;
        sceneObject.m_Mesh = arrow->GetMesh();

        const glm::mat4 translation = glm::translate(glm::mat4(1.f), _position);
        sceneObject.m_Transform = glm::scale(translation * MathsHelpers::GenerateRotationMatrixFromRight(_normal), glm::vec3(_length, 1.f, 1.f));
        sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);
        sceneObject.m_BaseColor = _color;

        m_UIDisplay->m_Debug3DDrawingQueue.emplace_back(zoneCoordinates, sceneObject);
    }
}

void UIDrawInterface::DebugDrawAABB(glm::ivec3 zoneCoordinates, const glm::vec3& _position, const AABB& _aabb)
{
    StaticMesh* box = AssetHandle<StaticMesh>(MeshAsset_UnitCube).GetAsset();

    if(box != nullptr)
    {
        Renderable::SceneObject sceneObject;
        sceneObject.m_Mesh = box->GetMesh();

        const glm::mat4 translation = glm::translate(glm::mat4(1.f), _position + _aabb.m_PositionOffset);
        sceneObject.m_Transform = glm::scale(translation,  2.f * _aabb.m_Dimensions);
        sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

        m_UIDisplay->m_Debug3DDrawingQueue.emplace_back(zoneCoordinates, sceneObject);
    }
}

bool UIDrawInterface::IsInAABB(glm::uvec2 _point, glm::uvec2 _boxPosition, glm::uvec2 _boxScale, UIAnchorPosition _anchor) const
{
    const glm::ivec2 displayResolution = GetDisplayResolution();

    const glm::uvec2 displayedPosition = GetScaledPosition<glm::uvec2>(displayResolution, _boxPosition.x, _boxPosition.y, _anchor);
    const glm::uvec2 displayedScale = GetScaledSize<glm::uvec2>(displayResolution, _boxScale.x, _boxScale.y);

    return _point.x >= displayedPosition.x
        && _point.y >= displayedPosition.y
        && _point.x <= displayedPosition.x + displayedScale.x
        && _point.y <= displayedPosition.y + displayedScale.y;
}
