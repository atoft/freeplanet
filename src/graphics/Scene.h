//
// Created by alastair on 04/10/18.
//


#pragma once

#include <glm/detail/type_mat4x4.hpp>
#include <array>
#include <vector>
#include <variant>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <src/engine/AssetHandle.h>

class ShaderProgram;
class Texture;

namespace Renderable
{
    enum class RenderMode
    {
        None,
        Normal,
        Wireframe,
        Background,
        Vista,
        Count
    };

    struct DirectionalLight
    {
        glm::vec3 m_Direction;
        glm::vec3 m_Color;
        float m_Intensity;
    };

    struct PointLight
    {
        glm::vec3 m_Origin;
        glm::vec4 m_Color;
        float m_Intensity;
    };

    struct AmbientLight
    {
        glm::vec3 m_Color;
        float m_Intensity;
    };

    struct Mesh
    {
        GLuint m_VaoHandle = 0;
        GLuint m_VboHandle = 0;
        GLuint m_EboHandle = 0;

        std::vector<GLint> m_VertexAttribs;

        u32 m_SizeofElements = 0;
    };

    struct SceneObject
    {
        // TODO Need some serious thought about how to make this both threadsafe and performant
        // Eg. would need to make every Handle operation locking and that's not sufficient
        //   If we get a pointer from the Handle then the array is reallocated
        AssetHandle<ShaderProgram> m_Shader;
        AssetHandle<Texture> m_Texture;

        Mesh m_Mesh;
        u32 m_MeshID = 0;

        glm::mat4 m_Transform;
    };

    struct Scene
    {
        // Usually projection * view * zoneOffset
        glm::mat4 m_CameraTransform;
        glm::vec3 m_CameraRelativePosition;

        DirectionalLight m_DirectionalLight;
        AmbientLight m_AmbientLight;
        std::vector<PointLight> m_PointLights;

        std::vector<SceneObject> m_SceneObjects;

        RenderMode m_RenderMode;

        glm::mat4 m_CameraRotation;
    };

    using DrawableVariant = std::variant<sf::RectangleShape, sf::Text, sf::Sprite>;
};

