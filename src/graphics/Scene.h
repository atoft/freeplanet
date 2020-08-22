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
#include <src/graphics/Color.h>
#include <src/graphics/GLHelpers.h>
#include <src/graphics/MeshType.h>

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
        Color m_Color;
        float m_Intensity;
    };

    struct AmbientLight
    {
        glm::vec3 m_Color;
        float m_Intensity;
    };

    struct Atmosphere
    {
        glm::vec3 m_Origin;
        f32 m_GroundRadius;
        f32 m_AtmosphereHeight;
        f32 m_AtmosphereBlendOutHeight;
    };

    struct Mesh
    {
        GLuint m_VaoHandle = 0;
        GLuint m_VboHandle = 0;
        GLuint m_EboHandle = 0;

        std::vector<GLint> m_VertexAttribs;

        u32 m_NumberOfElements = 0;
    };

    struct Material
    {
        AssetHandle<ShaderProgram> m_Shader;
        std::vector<std::pair<std::string, AssetHandle<Texture>>> m_Textures;
    };

    struct SceneObject
    {
        // TODO Need some serious thought about how to make this both threadsafe and performant
        // Eg. would need to make every Handle operation locking and that's not sufficient
        //   If we get a pointer from the Handle then the array is reallocated
        Material m_Material;

        Mesh m_Mesh;
        u32 m_MeshID = 0;
        MeshType m_MeshType = MeshType::Normal;

        Color m_BaseColor = Color(0.f, 0.f, 0.f, 1.f);

        glm::mat4 m_Transform;
    };

    struct Scene
    {
        // Usually view * zoneOffset
        glm::mat4 m_ViewTransform;
        glm::mat4 m_ProjectionTransform;

        glm::vec3 m_CameraRelativePosition;

        glm::mat4 m_CameraInverseProjection;

        DirectionalLight m_DirectionalLight;
        AmbientLight m_AmbientLight;
        std::vector<PointLight> m_PointLights;
        Atmosphere m_Atmosphere;

        std::vector<SceneObject> m_SceneObjects;

        RenderMode m_RenderMode;

        glm::vec3 m_LocalUpDirection;
    };

    using DrawableVariant = std::variant<sf::RectangleShape, sf::Text, sf::Sprite>;
};

