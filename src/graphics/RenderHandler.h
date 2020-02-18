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

    explicit RenderHandler(std::shared_ptr<sf::RenderWindow> _window);
    ~RenderHandler();

    void HandleEvent(EngineEvent _event);
    void Render(const World *world, const FreelookCameraComponent *c, const std::shared_ptr<UIDisplay> _uiDisplay);

private:
    void GenerateScenes(const World *world, const FreelookCameraComponent *c, std::vector<Renderable::Scene>& _scenes);
    void GenerateBoundingBoxScenes(
            const World *world,
            const FreelookCameraComponent *c,
            const std::vector<ZoneDebugRenderable>& _debugRenderables,
            std::vector<Renderable::Scene>& _scenes);
    void AddBoundingBoxObject(const glm::mat4 _transform, const StaticMesh* _mesh, Renderable::Scene& _outScene);

    void GenerateBackgroundScene(const World *world, const FreelookCameraComponent *c, std::vector<Renderable::Scene>& _scenes);

    void UpdateDynamicMesh(DynamicMeshHandle& _handle, const glm::mat4& _transform, const AssetHandle<ShaderProgram>& _shader, std::vector<Renderable::SceneObject>& _outSceneObjects);
    void OnDynamicMeshDestroyed(DynamicMeshID _id);

private:
    BoundingBoxMode m_BoundingBoxMode = BoundingBoxMode::None;
    bool m_ShouldRenderWireframe = false;
    bool m_ShouldRenderVista = true;

    // Make sure to release these in HandleEvent for quit
    AssetHandle<StaticMesh> m_UnitCube;
    AssetHandle<StaticMesh> m_UnitSphere;
    AssetHandle<StaticMesh> m_UnitCylinder;
    AssetHandle<StaticMesh> m_Quad;
    AssetHandle<StaticMesh> m_Arrow;

    AssetHandle<ShaderProgram> m_DefaultShader;
    AssetHandle<ShaderProgram> m_HACKTerrainShader;

    // TODO Get from world
    AssetHandle<Texture> m_SkyboxTexture;

    std::thread m_RenderThread;
    SceneRenderer m_SceneRenderer;

    glm::uvec2 m_WindowResolution;

    bool m_WaitingToQuit;

    // Keeping this around so we can generate debug information on the CPU.
    // It's a big waste of memory to have this here all the time, maybe we
    // should remove it later.
    RawMesh m_DebugZoneRawMeshForDebugDraw;
};
