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
#include <src/graphics/DynamicMeshID.h>
#include <src/graphics/GLHelpers.h>
#include <src/graphics/MeshType.h>
#include <src/graphics/RawMesh.h>
#include <src/graphics/ui/UITexture.h>

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

    enum class AlphaBlending
    {
        Opaque,
        Blend,
        Additive
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

        GLuint m_InstanceTransformsHandle;
        GLuint m_InstanceNormalTransformsHandle;
        GLuint m_InstanceColorsHandle;
        
        u32 m_NumberOfElements = 0;
    };

    struct Material
    {
        AssetHandle<ShaderProgram> m_Shader;
        std::vector<std::pair<std::string, AssetHandle<Texture>>> m_Textures;

        std::vector<std::pair<std::string, u32>> m_IntUniforms;
    };

    struct Solid
    {
        // TODO Need some serious thought about how to make this both threadsafe and performant
        // Eg. would need to make every Handle operation locking and that's not sufficient
        //   If we get a pointer from the Handle then the array is reallocated
        Material m_Material;

        Mesh m_Mesh;
        u32 m_MeshID = 0;
        MeshType m_MeshType = MeshType::Normal;
        AlphaBlending m_Blending = AlphaBlending::Opaque;

        Color m_BaseColor = Color(0.f, 0.f, 0.f, 1.f);
        bool m_NeedsDepthSort = false;
    };
    
    struct SceneObject
    {
        Solid m_Solid;
        glm::mat4 m_Transform;
    };

    struct SceneObjectInstanceData
    {
        glm::mat4 m_Transform;
        Color m_Color;
    };

    struct InstancedSceneObject
    {
        Solid m_Solid;
        std::vector<glm::mat4> m_Transforms;
        std::vector<Color> m_Colors;
        std::optional<glm::vec3> m_SortOrigin;
        
        // TODO Not great to have this here.
        std::vector<SceneObjectInstanceData> m_InstanceDataForSorting;
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
        std::vector<InstancedSceneObject> m_Instances;
        
        RenderMode m_RenderMode;

        glm::vec3 m_LocalUpDirection;
    };

    using DrawableVariant = std::variant<sf::RectangleShape, sf::Text, sf::Sprite, UISprite>;

    struct MeshRequest
    {
        DynamicMeshID m_ID;
        RawMesh m_PendingMesh;
    };

    struct Frame
    {
        std::vector<Renderable::Scene> m_PendingScenes;
        std::vector<Renderable::DrawableVariant> m_PendingUIElements;

        std::vector<MeshRequest> m_MeshCreationRequests;
        std::vector<DynamicMeshID> m_MeshDestructionRequests;
    };
};

