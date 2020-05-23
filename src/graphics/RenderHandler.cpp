//
// Created by alastair on 25/03/17.
//

#include "RenderHandler.h"

#include <memory>

#include <src/engine/EngineConfig.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/FreelookCameraComponent.h>
#include <src/world/World.h>
#include <src/world/WorldZone.h>
#include <src/world/WorldObject.h>
#include <src/world/vista/VistaHandler.h>
#include <src/profiling/Profiler.h>

RenderHandler::RenderHandler(std::shared_ptr<sf::RenderWindow> _window, GraphicsConfig _config)
{
    m_UnitCube = AssetHandle<StaticMesh>(MeshAsset_UnitCube);
    m_UnitSphere = AssetHandle<StaticMesh>(MeshAsset_UnitUVSphere);
    m_UnitCylinder = AssetHandle<StaticMesh>(MeshAsset_UnitCylinder);
    m_Quad = AssetHandle<StaticMesh>(MeshAsset_UnitQuad);
    m_Arrow = AssetHandle<StaticMesh>(MeshAsset_Arrow);
    m_SkyboxTexture = AssetHandle<Texture>(TextureAsset_Cubemap_Dev);

    // We hold a Handle to the default shader to keep it loaded, we use it to
    // initialise meshes in the Render thread
    m_DefaultShader = AssetHandle<ShaderProgram>(ShaderAsset_Default);

    //HACK
    m_HACKTerrainShader = AssetHandle<ShaderProgram>(ShaderAsset_Terrain_Base);

    m_RenderThread = std::thread([this, _window]()
                                 { m_SceneRenderer.Run(_window); });

    m_WaitingToQuit = false;

    m_WindowResolution.x = _window->getSize().x;
    m_WindowResolution.y = _window->getSize().y;
    m_DefaultFov = glm::radians(glm::clamp(_config.m_FieldOfViewDegrees, 45.f, 170.f));
}

RenderHandler::~RenderHandler()
{
    m_SceneRenderer.RequestTerminate();
    m_RenderThread.join();
}

void RenderHandler::HandleEvent(EngineEvent _event)
{
    m_SceneRenderer.HandleEvent(_event);

    switch (_event.GetType())
    {
    case EngineEvent::Type::OnQuit:
    {
        m_Quad = AssetHandle<StaticMesh>();
        m_UnitCube = AssetHandle<StaticMesh>();
        m_UnitSphere = AssetHandle<StaticMesh>();
        m_UnitCylinder = AssetHandle<StaticMesh>();
        m_Arrow = AssetHandle<StaticMesh>();
        m_WaitingToQuit = true;

        break;
    }
    case EngineEvent::Type::RenderSetBoundingBoxes:
    {
        m_BoundingBoxMode = static_cast<BoundingBoxMode>(_event.GetIntData());
        break;
    }
    case EngineEvent::Type::RenderSetWireframe:
    {
        m_ShouldRenderWireframe = static_cast<bool>(_event.GetIntData());
        break;
    }
    case EngineEvent::Type::RenderSetVista:
    {
        m_ShouldRenderVista = static_cast<bool>(_event.GetIntData());
        break;
    }
    case EngineEvent::Type::RenderOnDynamicMeshDestroyed:
    {
        OnDynamicMeshDestroyed(static_cast<DynamicMeshID>(_event.GetIntData()));
        break;
    }
    default:
        break;
    }

}

void RenderHandler::Render(const World* _world, std::shared_ptr<const UIDisplay> _uiDisplay)
{
    ProfileCurrentFunction();

    if (m_WaitingToQuit)
    {
        LogMessage("Requesting the final frame");
    }

    std::vector<Renderable::Scene> pendingScenes;
    std::vector<Renderable::DrawableVariant> pendingUIElements;

    if (_world != nullptr)
    {
        const FreelookCameraComponent* camera = _world->GetPlayerHandler()->GetLocalCamera();

        if (camera != nullptr)
        {
            GenerateBackgroundScene(_world, camera, pendingScenes);
            GenerateScenes(_world, camera, pendingScenes);

            GenerateBoundingBoxScenes(_world, camera, _uiDisplay->m_Debug3DDrawingQueue, pendingScenes);
        }
    }

    if (_uiDisplay != nullptr)
    {
        pendingUIElements = _uiDisplay->m_DrawingQueue;
    }

    {
        ProfileCurrentScope("Waiting for SceneRenderer");
        m_SceneRenderer.RequestRender(pendingScenes, pendingUIElements);
    }
}

void RenderHandler::GenerateScenes(const World* world, const FreelookCameraComponent* c,
                                   std::vector<Renderable::Scene>& _scenes)
{
    ProfileCurrentFunction();

    // TODO Refactor copy paste.
    if (m_ShouldRenderVista)
    {
        Renderable::Scene sceneToRender;
        sceneToRender.m_RenderMode = m_ShouldRenderWireframe ? Renderable::RenderMode::Wireframe
                                                             : Renderable::RenderMode::Vista;

        glm::mat4 view = glm::inverse(c->GetCameraZoneTransform());
        glm::mat4 projection = glm::perspective(m_DefaultFov,
            static_cast<f32>(m_WindowResolution.x) / static_cast<f32>(m_WindowResolution.y),
            0.1f,
            c->GetFarClipDistance());

        sceneToRender.m_CameraTransform =
                projection * view;

        sceneToRender.m_CameraRelativePosition = MathsHelpers::GetPosition(c->GetCameraZoneTransform());

        sceneToRender.m_DirectionalLight.m_Direction = world->GetEnvironmentState().GetSunDirection();
        sceneToRender.m_DirectionalLight.m_Color = world->GetEnvironmentState().GetSunColor();
        sceneToRender.m_DirectionalLight.m_Intensity = world->GetEnvironmentState().GetSunIntensity();

        sceneToRender.m_AmbientLight.m_Color = world->GetEnvironmentState().GetAmbientColor();
        sceneToRender.m_AmbientLight.m_Intensity = world->GetEnvironmentState().GetAmbientIntensity();

        UpdateDynamicMesh(world->GetVistaHandler()->m_DynamicMesh, world->GetVistaHandler()->GetTerrainModelTransform(), m_HACKTerrainShader, sceneToRender.m_SceneObjects);
        _scenes.push_back(sceneToRender);
    }

    for (const WorldZone& zone : world->GetActiveZones())
    {
        Renderable::Scene sceneToRender;
        sceneToRender.m_RenderMode = m_ShouldRenderWireframe ? Renderable::RenderMode::Wireframe
                                                             : Renderable::RenderMode::Normal;

        glm::mat4 view = glm::inverse(c->GetCameraZoneTransform());
        glm::mat4 projection = glm::perspective(m_DefaultFov,
            static_cast<f32>(m_WindowResolution.x) / static_cast<f32>(m_WindowResolution.y),
            0.1f,
            c->GetFarClipDistance());

        sceneToRender.m_CameraTransform =
                projection * view * zone.GetRelativeTransform(c->GetOwnerObject()->GetRef().m_ZoneCoordinates);

        sceneToRender.m_CameraRelativePosition = MathsHelpers::GetPosition(c->GetCameraZoneTransform());

        sceneToRender.m_DirectionalLight.m_Direction = world->GetEnvironmentState().GetSunDirection();
        sceneToRender.m_DirectionalLight.m_Color = world->GetEnvironmentState().GetSunColor();
        sceneToRender.m_DirectionalLight.m_Intensity = world->GetEnvironmentState().GetSunIntensity();

        sceneToRender.m_AmbientLight.m_Color = world->GetEnvironmentState().GetAmbientColor();
        sceneToRender.m_AmbientLight.m_Intensity = world->GetEnvironmentState().GetAmbientIntensity();

        for (const RenderComponent& component : zone.GetComponents<RenderComponent>())
        {
            const WorldObject* worldObject = component.GetOwnerObject();
            assert(worldObject != nullptr);

            Renderable::SceneObject sceneObject;
            sceneObject.m_Transform = worldObject->GetZoneTransform();
            sceneObject.m_Shader = component.GetShader();
            sceneObject.m_Texture = component.GetTexture();

            const StaticMesh* mesh = component.GetMesh().GetAsset();
            if (mesh != nullptr)
            {
                sceneObject.m_Mesh = mesh->GetMesh();
            }
            else
            {
                LogError("StaticMesh wasn't loaded");
            }

            sceneToRender.m_SceneObjects.push_back(sceneObject);
        }

        UpdateDynamicMesh(zone.GetTerrainComponent().m_DynamicMesh, zone.GetTerrainModelTransform(), m_HACKTerrainShader, sceneToRender.m_SceneObjects);

        if (zone.ContainsPlayer())
        {
            m_DebugZoneRawMeshForDebugDraw = zone.GetTerrainComponent().m_DynamicMesh.m_RawMesh;
        }

        _scenes.push_back(sceneToRender);
    }
}

void RenderHandler::UpdateDynamicMesh(DynamicMeshHandle& _handle, const glm::mat4& _transform, const AssetHandle<ShaderProgram>& _shader, std::vector<Renderable::SceneObject>& _outSceneObjects)
{
    if (_handle.m_DynamicMeshId == DYNAMICMESHID_INVALID)
    {
        // The GPU DynamicMesh needs updating.

        if (_handle.m_PreviousDynamicMeshId != DYNAMICMESHID_INVALID)
        {
            // This frame, we will render the previous version of the mesh while we request a new one.
            Renderable::SceneObject sceneObject;
            sceneObject.m_Transform = _transform;
            sceneObject.m_Shader = _shader; // TODO Somewhere to source this for DynamicMeshes
            // TODO texture

            sceneObject.m_MeshID = _handle.m_PreviousDynamicMeshId;

            _outSceneObjects.push_back(sceneObject);
        }

        // Make the request for a new GPU mesh.
        if (!_handle.m_RawMesh.IsEmpty())
        {
            _handle.m_DynamicMeshId = m_SceneRenderer.RequestDynamicMeshCreation(_handle.m_RawMesh);
        }
    }
    else
    {
        // The mesh is up-to-date, just need to render it.

        if (_handle.m_PreviousDynamicMeshId != DYNAMICMESHID_INVALID)
        {
            // We don't need the old mesh any more, we can clean it up.
            m_SceneRenderer.RequestDynamicMeshDestruction(_handle.m_PreviousDynamicMeshId);
            _handle.m_PreviousDynamicMeshId = DYNAMICMESHID_INVALID;

        }

        Renderable::SceneObject sceneObject;
        sceneObject.m_Transform = _transform;
        sceneObject.m_Shader = _shader; // TODO Somewhere to source this for DynamicMeshes
        // TODO texture

        sceneObject.m_MeshID = _handle.m_DynamicMeshId;

        _outSceneObjects.push_back(sceneObject);
    }
}

void RenderHandler::OnDynamicMeshDestroyed(DynamicMeshID _id)
{
    if (_id != DYNAMICMESHID_INVALID)
    {
        m_SceneRenderer.RequestDynamicMeshDestruction(_id);
        _id = DYNAMICMESHID_INVALID;

    }
}

void RenderHandler::GenerateBoundingBoxScenes(const World* world, const FreelookCameraComponent* c,
                                              const std::vector<ZoneDebugRenderable>& _debugRenderables,
                                              std::vector<Renderable::Scene>& _scenes)
{
    ProfileCurrentFunction();

    const StaticMesh* cubeMesh = m_UnitCube.GetAsset();
    const StaticMesh* sphereMesh = m_UnitSphere.GetAsset();
    const StaticMesh* cylinderMesh = m_UnitCylinder.GetAsset();

    for (const WorldZone& zone : world->GetActiveZones())
    {
        Renderable::Scene sceneToRender;
        sceneToRender.m_RenderMode = Renderable::RenderMode::Wireframe;
        glm::mat4 view = glm::inverse(c->GetCameraZoneTransform());
        glm::mat4 projection = glm::perspective(m_DefaultFov,
                static_cast<f32>(m_WindowResolution.x) / static_cast<f32>(m_WindowResolution.y),
                0.1f,
                c->GetFarClipDistance());

        sceneToRender.m_CameraTransform =
                projection * view * zone.GetRelativeTransform(c->GetOwnerObject()->GetWorldPosition().m_ZoneCoordinates);

        if (m_BoundingBoxMode == BoundingBoxMode::Components)
        {
            for (const ColliderComponent& collider : zone.GetComponents<ColliderComponent>())
            {
                const WorldObject* worldObject = collider.GetOwnerObject();
                assert(worldObject != nullptr);

                switch (collider.m_CollisionPrimitiveType)
                {
                case CollisionPrimitiveType::OBB:
                    [[fallthrough]];
                case CollisionPrimitiveType::AABB:  // Currently we don't actually support AABBs, they just act like OBBs if the object is rotated...
                {
                    AddBoundingBoxObject(
                            glm::scale(
                                    glm::translate(glm::mat4(1.f), worldObject->GetPosition() + collider.m_Bounds.m_PositionOffset)
                                    * MathsHelpers::GetRotationMatrix(worldObject->GetZoneTransform()), collider.m_Bounds.m_Dimensions * 2.f),
                            cubeMesh, sceneToRender);
                    break;
                }
                case CollisionPrimitiveType::SPHERE:
                {
                    AddBoundingBoxObject(
                            glm::scale(
                                    glm::translate(glm::mat4(1.f), worldObject->GetPosition() + collider.m_Bounds.m_PositionOffset)
                                    * MathsHelpers::GetRotationMatrix(worldObject->GetZoneTransform()), collider.m_Bounds.m_Dimensions * 2.f),
                            sphereMesh, sceneToRender);
                    break;
                }
                case CollisionPrimitiveType::CAPSULE:
                {
                    glm::vec3 cylinderScale = glm::vec3(
                            collider.m_Bounds.m_Dimensions.x,
                            collider.m_Bounds.m_Dimensions.y - collider.m_Bounds.m_Dimensions.x,
                            collider.m_Bounds.m_Dimensions.z) * 2.f;
                    AddBoundingBoxObject(glm::scale(worldObject->GetZoneTransform(), cylinderScale), cylinderMesh,
                                         sceneToRender);

                    glm::vec3 sphereScale = glm::vec3(collider.m_Bounds.m_Dimensions.x) * 2.f;
                    glm::vec3 sphereOffset = glm::vec3(0, collider.m_Bounds.m_Dimensions.y -
                                                          collider.m_Bounds.m_Dimensions.x, 0);

                    AddBoundingBoxObject(
                            glm::scale(glm::translate(worldObject->GetZoneTransform(), sphereOffset), sphereScale),
                            sphereMesh, sceneToRender);
                    AddBoundingBoxObject(
                            glm::scale(glm::translate(worldObject->GetZoneTransform(), -sphereOffset), sphereScale),
                            sphereMesh, sceneToRender);


                    break;
                }
                default:
                {
                    AddBoundingBoxObject(
                            glm::scale(worldObject->GetZoneTransform(), collider.m_Bounds.m_Dimensions), cubeMesh,
                            sceneToRender);
                    break;
                }
                }
            }
        }
        else if (m_BoundingBoxMode == BoundingBoxMode::Zones)
        {
            Renderable::SceneObject sceneObject;
            sceneObject.m_Transform = glm::scale(glm::mat4(1.f), zone.GetDimensions());
            sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

            if (cubeMesh != nullptr)
            {
                sceneObject.m_Mesh = cubeMesh->GetMesh();
            }

            sceneToRender.m_SceneObjects.push_back(sceneObject);
        }
        else if (m_BoundingBoxMode == BoundingBoxMode::Chunks)
        {
            if (zone.ContainsPlayer())
            {
                const u32 chunksPerAxis = zone.GetTerrainComponent().GetDimensions();
                const glm::vec3 chunkScale = zone.GetDimensions() / static_cast<f32>(chunksPerAxis);

                for (u32 z = 0; z < chunksPerAxis; ++z)
                {
                    for (u32 y = 0; y < chunksPerAxis; ++y)
                    {
                        for (u32 x = 0; x < chunksPerAxis; ++x)
                        {
                            Renderable::SceneObject sceneObject;
                            sceneObject.m_Transform = zone.GetTerrainModelTransform() * glm::scale(
                                    glm::translate(glm::mat4(1.f), glm::vec3(x, y, z) * chunkScale), chunkScale);
                            sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

                            if (cubeMesh != nullptr)
                            {
                                sceneObject.m_Mesh = cubeMesh->GetMesh();
                            }

                            sceneToRender.m_SceneObjects.push_back(sceneObject);
                        }
                    }
                }
            }
        }
        else if (m_BoundingBoxMode == BoundingBoxMode::Normals)
        {
            if (zone.ContainsPlayer())
            {
                const StaticMesh* arrowMesh = m_Arrow.GetAsset();

                u32 vertexIdx = 0;
                for (const glm::vec3& vertex : m_DebugZoneRawMeshForDebugDraw.m_Vertices)
                {
                    const glm::vec3& normal = m_DebugZoneRawMeshForDebugDraw.m_Normals[vertexIdx];

                    Renderable::SceneObject sceneObject;
                    sceneObject.m_Transform = zone.GetTerrainModelTransform() *
                            glm::translate(glm::mat4x4(1.f), vertex)
                            * MathsHelpers::GenerateRotationMatrixFromNormal(normal);
                    sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

                    if (arrowMesh != nullptr)
                    {
                        sceneObject.m_Mesh = arrowMesh->GetMesh();
                    }

                    sceneToRender.m_SceneObjects.push_back(sceneObject);
                    ++vertexIdx;
                }
            }
        }

        for (const ZoneDebugRenderable& debugRenderable : _debugRenderables)
        {
            if (debugRenderable.first == zone.GetCoordinates())
            {
                sceneToRender.m_SceneObjects.push_back(debugRenderable.second);
            }
        }

        _scenes.push_back(sceneToRender);
    }
}

void RenderHandler::AddBoundingBoxObject(const glm::mat4 _transform, const StaticMesh* _mesh, Renderable::Scene& _outScene)
{
    Renderable::SceneObject sceneObject;
    sceneObject.m_Transform = _transform;
    sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

    if (_mesh != nullptr)
    {
        sceneObject.m_Mesh = _mesh->GetMesh();
    }
    else
    {
        LogError("StaticMesh wasn't loaded or was unavailable for the CollisionPrimitiveType.");
    }
    _outScene.m_SceneObjects.push_back(sceneObject);
}

void RenderHandler::GenerateBackgroundScene(const World* world, const FreelookCameraComponent* c,
                                            std::vector<Renderable::Scene>& _scenes)
{
    ProfileCurrentFunction();

    const StaticMesh* quadMesh = m_Quad.GetAsset();

    Renderable::Scene sceneToRender;
    sceneToRender.m_RenderMode = Renderable::RenderMode::Background;

    sceneToRender.m_CameraTransform = glm::mat4(1.f);
    sceneToRender.m_CameraRotation = MathsHelpers::GetRotationMatrix(c->GetCameraZoneTransform());

    sceneToRender.m_CameraInverseProjection = glm::inverse(glm::perspective(m_DefaultFov,
                                                               static_cast<f32>(m_WindowResolution.x) / static_cast<f32>(m_WindowResolution.y),
                                                               0.1f,
                                                               c->GetFarClipDistance()) * glm::inverse(sceneToRender.m_CameraRotation));

    sceneToRender.m_DirectionalLight.m_Direction = world->GetEnvironmentState().GetSunDirection();
    sceneToRender.m_DirectionalLight.m_Color = world->GetEnvironmentState().GetSunColor();
    sceneToRender.m_DirectionalLight.m_Intensity = world->GetEnvironmentState().GetSunIntensity();

    sceneToRender.m_AmbientLight.m_Color = world->GetEnvironmentState().GetAmbientColor();
    sceneToRender.m_AmbientLight.m_Intensity = world->GetEnvironmentState().GetAmbientIntensity();

    const Planet* planet = world->GetPlanet();

    if (planet != nullptr)
    {
        // TODO clean up this calculation.
        sceneToRender.m_Atmosphere.m_Origin = WorldPosition(planet->m_OriginZone, glm::vec3(0.f)).GetPositionRelativeTo(c->GetOwnerObject()->GetRef().m_ZoneCoordinates)
                - c->GetOwnerObject()->GetPosition();
        sceneToRender.m_Atmosphere.m_GroundRadius = planet->m_Radius;
        sceneToRender.m_Atmosphere.m_AtmosphereHeight = planet->m_AtmosphereHeight;
        sceneToRender.m_Atmosphere.m_AtmosphereBlendOutHeight = planet->m_BlendOutHeight;
    }

    Renderable::SceneObject sceneObject;
    sceneObject.m_Transform = glm::mat4(1.f);
    if (quadMesh != nullptr)
    {
        sceneObject.m_Mesh = quadMesh->GetMesh();
    }
    sceneObject.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Skybox);

    sceneToRender.m_SceneObjects.emplace_back(sceneObject);

    _scenes.push_back(sceneToRender);
}
