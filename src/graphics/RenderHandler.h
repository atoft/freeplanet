//
// Created by alastair on 25/03/17.
//

#pragma once

#include <thread>
#include <glm/gtc/matrix_transform.hpp>

#include <src/tools/globals.h>
#include <src/graphics/Scene.h>
#include <src/graphics/SceneRenderer.h>
#include <src/graphics/ui/UIDisplay.h>

class World;
class FreelookCameraComponent;
class DynamicMeshHandle;
struct GraphicsConfig;

// Manages the active SceneRenderer and acts as an interface between gameplay types
// and rendering
class RenderHandler
{
public:
    enum class BoundingBoxMode
    {
        None = 0,
        Components = 1,
        Zones = 2,
        Chunks = 3,
        Normals = 4
    };

    explicit RenderHandler(std::shared_ptr<sf::RenderWindow> _window, GraphicsConfig _config);
    ~RenderHandler();

    void HandleEvent(EngineEvent _event);
    void Render(const World* _world, std::shared_ptr<const UIDisplay> _uiDisplay);

private:
    void GenerateScenes(const World* _world, const FreelookCameraComponent* _camera, Renderable::Frame& _inOutFrame);
    void GenerateBoundingBoxScenes(
            const World *_world,
            const FreelookCameraComponent *_camera,
            const std::vector<ZoneDebugRenderable>& _debugRenderables,
            std::vector<Renderable::Scene>& _scenes);
    void GenerateBackgroundScene(const World *_world, const FreelookCameraComponent *_camera, std::vector<Renderable::Scene>& _scenes) const;

    void AddBoundingBoxObject(const glm::mat4 _transform, const StaticMesh* _mesh, Renderable::Scene& _outScene) const;

    void GenerateSceneCamera(const World* _world, const FreelookCameraComponent* _camera, Renderable::Scene& _outScene) const;
    void GenerateSceneGlobalLighting(const World* _world, const FreelookCameraComponent* _camera, Renderable::Scene& _outScene) const;

    void UpdateSharedDynamicMeshes(const World* _world, Renderable::Frame& _inOutFrame);
    
    void UpdateDynamicMesh(DynamicMeshHandle& _handle, const glm::mat4& _transform,
                           const AssetHandle<ShaderProgram>& _shader,
                           std::vector<Renderable::SceneObject>& _outSceneObjects,
                           Renderable::Frame& _inOutFrame,
                           u32 _terrainLOD);
    void OnDynamicMeshDestroyed(DynamicMeshID _id);

private:
    BoundingBoxMode m_BoundingBoxMode = BoundingBoxMode::None;
    bool m_ShouldRenderWireframe = false;
    bool m_ShouldRenderVista = true;
    u32 m_MaxTerrainLOD = 1;
    bool m_ShouldRenderParticles = true;

    // Make sure to release these in HandleEvent for quit
    AssetHandle<StaticMesh> m_UnitCube;
    AssetHandle<StaticMesh> m_UnitSphere;
    AssetHandle<StaticMesh> m_UnitCylinder;
    AssetHandle<StaticMesh> m_Quad;
    AssetHandle<StaticMesh> m_Arrow;

    AssetHandle<ShaderProgram> m_DefaultShader;
    AssetHandle<ShaderProgram> m_HACKTerrainShader;

    AssetHandle<Texture> m_HACKTerrainVolumeTexture0;
    AssetHandle<Texture> m_HACKTerrainVolumeTexture1;

    // TODO Get from world
    AssetHandle<Texture> m_SkyboxTexture;

    std::thread m_RenderThread;
    SceneRenderer m_SceneRenderer;

    DynamicMeshID m_NextAvailableDynamicMeshID = 1;

    // Requests coming from a handle being destroyed, rather than the mesh being updated.
    std::vector<DynamicMeshID> m_ExtraMeshDesructionRequests;

    glm::uvec2 m_WindowResolution;
    f32 m_DefaultFov = 0.f;

    bool m_WaitingToQuit = false;

    // Keeping this around so we can generate debug information on the CPU.
    // It's a big waste of memory to have this here all the time, maybe we
    // should remove it later.
    RawMesh m_DebugZoneRawMeshForDebugDraw;
};
