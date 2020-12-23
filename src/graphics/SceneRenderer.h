/*
 * Copyright 2017-2020 Alastair Toft
 *
 * This file is part of freeplanet.
 *
 * freeplanet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeplanet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeplanet. If not, see <http://www.gnu.org/licenses/>.
 */

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

    void RequestRender(Renderable::Frame _frame);
    void RequestTerminate();



private:
    static void Init();
    void PrepareRender();
    static void PrepareWireframeRender();
    static void ClearScreen();
    static void PostRender();
    void Render(Renderable::Scene& _scene, std::shared_ptr<sf::RenderWindow> _window);
    void Render(std::vector<Renderable::Scene> _scenes, std::shared_ptr<sf::RenderWindow> _window);
    void HandleDynamicMeshRequests();
    static void Render(std::vector<Renderable::DrawableVariant> _uiElements, std::shared_ptr<sf::RenderWindow> _window);

    const Renderable::Mesh* BindMesh(const Renderable::Solid& _solid) const;
    void UnbindMesh(const Renderable::Mesh* _mesh) const;
    ShaderProgram* UseShaderWithMesh(const Renderable::Solid& _solid, const Renderable::Mesh* _mesh) const;
    glm::mat4 ComputeTransform(const Renderable::Scene& _scene, const Renderable::Solid& _solid, const glm::mat4& _objectTransform) const;
    void SetSceneShaderParameters(const Renderable::Scene& _scene, ShaderProgram* _shaderProgram, const std::shared_ptr<const sf::RenderWindow> _window) const;
    void SetupTextures(ShaderProgram* _shaderProgram, const Renderable::Solid& _solid) const;
    void UnbindTextures(ShaderProgram* _shaderProgram, const Renderable::Solid& _solid) const;
    void SetMaterialShaderParameters(ShaderProgram* _shaderProgram, const Renderable::Solid& _solid) const;
    void UpdateBlending(Renderable::AlphaBlending _blending);
    
private:
    bool m_IsFirstFrame = true;

    EventHandler<EngineEvent> m_EventHandler;
    std::vector<Renderable::Scene> m_CurrentScenes;
    std::vector<Renderable::DrawableVariant> m_CurrentUIElements;
    AssetLoader<ShaderProgram> m_ShaderLoader;
    AssetLoader<Texture> m_TextureLoader;
    AssetLoader<StaticMesh> m_MeshLoader;
    AssetLoader<UITexture> m_UITextureLoader;
    Renderable::AlphaBlending m_CurrentBlending = Renderable::AlphaBlending::Opaque;

    std::mutex m_RenderMutex;
    std::condition_variable m_StartRenderCV;
    bool m_ReadyToRender;
    bool m_RenderComplete;

    std::vector<Renderable::MeshRequest> m_MeshCreationRequests;
    std::vector<DynamicMeshID > m_MeshDestructionRequests;
    std::map<DynamicMeshID, DynamicMesh> m_LoadedDynamicMeshes;
};
