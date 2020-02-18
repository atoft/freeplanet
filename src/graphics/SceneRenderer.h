//
// Created by alastair on 05/10/18.
//

#pragma once

#include <condition_variable>

#include <src/world/GeometryTypes.h>
#include <src/graphics/Scene.h>
#include <src/engine/events/EventHandler.h>
#include <src/graphics/StaticMesh.h>
#include <src/engine/AssetLoader.h>
#include <src/graphics/DynamicMesh.h>
#include <src/graphics/RawMesh.h>
#include <src/engine/events/EngineEvent.h>

namespace sf
{
    class RenderWindow;
}

class SceneRenderer
{
public:
    void Run(std::shared_ptr<sf::RenderWindow> _window);
    void HandleEvent(EngineEvent _event);

    void RequestRender(std::vector<Renderable::Scene> _scenes, std::vector<Renderable::DrawableVariant> _uiElements);
    void RequestTerminate();

    DynamicMeshID RequestDynamicMeshCreation(const RawMesh &_mesh);
    void RequestDynamicMeshDestruction(DynamicMeshID _meshID);

private:
    static void Init();
    static void PrepareRender();
    static void PrepareWireframeRender();
    static void ClearScreen();
    static void PostRender();
    void Render(Renderable::Scene& _scene, std::shared_ptr<sf::RenderWindow> _window);
    void Render(std::vector<Renderable::Scene> _scenes, std::shared_ptr<sf::RenderWindow> _window);
    static void Render(std::vector<Renderable::DrawableVariant> _uiElements, std::shared_ptr<sf::RenderWindow> _window);

private:
    bool m_IsFirstFrame = true;

    EventHandler<EngineEvent> m_EventHandler;
    std::vector<Renderable::Scene> m_CurrentScenes;
    std::vector<Renderable::DrawableVariant> m_CurrentUIElements;
    AssetLoader<ShaderProgram> m_ShaderLoader;
    AssetLoader<Texture> m_TextureLoader;
    AssetLoader<StaticMesh> m_MeshLoader;

    std::mutex m_RenderMutex;
    std::condition_variable m_StartRenderCV;
    bool m_ReadyToRender;
    bool m_RenderComplete;

    struct MeshRequest
    {
        DynamicMeshID m_ID;
        RawMesh m_PendingMesh;
    };

    std::vector<MeshRequest> m_MeshCreationRequests;
    std::vector<DynamicMeshID > m_MeshDestructionRequests;
    DynamicMeshID m_NextAvailableID = 1;
    std::map<DynamicMeshID, DynamicMesh> m_LoadedDynamicMeshes;
};
