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

static const glm::ivec2 DEFAULT_RES = glm::ivec2(1920,1080);

glm::ivec2 UIDrawInterface::GetDisplayResolution() const
{
    return glm::ivec2(m_UIDisplay->m_Window->getSize().x, m_UIDisplay->m_Window->getSize().y);
}

void UIDrawInterface::DrawString(
        glm::vec2 _position,
        std::string _text,
        f32 _scale,
        Color _color,
        FontStyle _style,
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

    const f32 scale = _exactPixels ? _scale : GetScaledVector(_scale, _scale).y;

    sfText.setCharacterSize(scale);
    sfText.setPosition(
            _exactPixels ? _position.x : (_position.x / DEFAULT_RES.x) * m_UIDisplay->m_Window->getSize().x,
            _exactPixels ? _position.y : (_position.y / DEFAULT_RES.y) * m_UIDisplay->m_Window->getSize().y);

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
                                _exactPixels ? _position.y : GetScaledVector(_position.x, _position.y).y);

        m_UIDisplay->m_DrawingQueue.push_back(sfRectangle);
    }
}

void UIDrawInterface::DrawRectangle(glm::vec2 _position, glm::vec2 _scale, Color _color)
{
    assert(m_UIDisplay != nullptr);
    assert(m_UIDisplay->m_Window != nullptr);

    sf::RectangleShape sfRectangle(GetSFMLScaledVector(_scale.x, _scale.y));

    sfRectangle.setFillColor(
            sf::Color(
                    _color.r * UIDisplay::MAX_COLOR,
                    _color.g * UIDisplay::MAX_COLOR,
                    _color.b * UIDisplay::MAX_COLOR,
                    _color.a * UIDisplay::MAX_COLOR));



    sfRectangle.setPosition(GetSFMLScaledVector(_position.x, _position.y));

    m_UIDisplay->m_DrawingQueue.push_back(sfRectangle);
}

// This is a slow function as the image is not cached.
// In future we could add Asset handling for SFML textures if we're using it a lot
void UIDrawInterface::DrawSpriteFromDisk(glm::vec2 _position, glm::vec2 _scale, std::string _path)
{
    // TODO I think this is broken because the texture needs to be created on the Render thread
    sf::Texture splashBg;
    bool loadSuccess = splashBg.loadFromFile(Globals::FREEPLANET_ASSET_PATH + _path);
    if(!loadSuccess)
    {
        LogError("Failed to load UI texture " + _path);
        return;
    }
    sf::Sprite sprite;
    sprite.setOrigin((_position.x / DEFAULT_RES.x) * m_UIDisplay->m_Window->getSize().x,
                     (_position.y / DEFAULT_RES.y) * m_UIDisplay->m_Window->getSize().y);

    sprite.setScale(1.f,1.f);//(_scale.x / DEFAULT_RES.x) * m_UIDisplay->m_Window->getSize().x,(_scale.y / DEFAULT_RES.y) * m_UIDisplay->m_Window->getSize().y);
    sprite.setTexture(splashBg);

    m_UIDisplay->m_DrawingQueue.push_back(sprite);
}

void UIDrawInterface::DebugDrawSphere(glm::ivec3 zoneCoordinates, glm::vec3 _position, f32 _radius)
{
    StaticMesh* sphere = AssetHandle<StaticMesh>(MeshAsset_UnitUVSphere).GetAsset();

    if(sphere != nullptr)
    {
        Renderable::SceneObject sceneObject;
        sceneObject.m_Mesh = sphere->GetMesh();
        sceneObject.m_Transform = glm::scale(glm::translate(glm::mat4x4(1.f), _position),  glm::vec3(_radius));
        sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

        m_UIDisplay->m_Debug3DDrawingQueue.emplace_back(zoneCoordinates, sceneObject);
    }
}

void UIDrawInterface::DebugDrawArrow(glm::ivec3 zoneCoordinates, glm::vec3 _position, f32 _length, glm::vec3 _normal)
{
    StaticMesh* arrow = AssetHandle<StaticMesh>(MeshAsset_Arrow).GetAsset();

    if(arrow != nullptr)
    {
        Renderable::SceneObject sceneObject;
        sceneObject.m_Mesh = arrow->GetMesh();

        const glm::mat4 translation = glm::translate(glm::mat4(1.f), _position);
        sceneObject.m_Transform = glm::scale(translation * MathsHelpers::GenerateRotationMatrixFromNormal(_normal),  glm::vec3(_length, 1.f, 1.f));
        sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

        m_UIDisplay->m_Debug3DDrawingQueue.emplace_back(zoneCoordinates, sceneObject);
    }
}

glm::uvec2 UIDrawInterface::GetScaledVector(u32 x, u32 y) const
{
    glm::uvec2 result;
    result.x = (static_cast<f32>(x) / static_cast<f32>(DEFAULT_RES.x)) * m_UIDisplay->m_Window->getSize().x;
    result.y = (static_cast<f32>(y) / static_cast<f32>(DEFAULT_RES.y)) * m_UIDisplay->m_Window->getSize().y;
    return result;
}

sf::Vector2f UIDrawInterface::GetSFMLScaledVector(u32 x, u32 y) const
{
    sf::Vector2f result;
    result.x = (static_cast<f32>(x) / static_cast<f32>(DEFAULT_RES.x)) * m_UIDisplay->m_Window->getSize().x;
    result.y = (static_cast<f32>(y) / static_cast<f32>(DEFAULT_RES.y)) * m_UIDisplay->m_Window->getSize().y;
    return result;
}
