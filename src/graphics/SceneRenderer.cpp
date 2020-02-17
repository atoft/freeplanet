//
// Created by alastair on 05/10/18.
//

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <src/tools/MathsHelpers.h>
#include <src/graphics/SceneRenderer.h>
#include <src/graphics/ShaderProgram.h>
#include <src/graphics/Texture.h>

void SceneRenderer::Run(std::shared_ptr<sf::RenderWindow> _window)
{
    ThreadUtils::tl_ThreadType = ThreadType::Render;

    _window->setActive(true);
    Init();

    bool quitRequested = false;

    m_RenderComplete = true;

    while (!quitRequested)
    {
        std::vector<EngineEvent> allEvents = m_EventHandler.PopEvents();

        for(const EngineEvent& event : allEvents)
        {
            if(event.GetType() == EngineEvent::Type::OnQuit)
            {
                quitRequested = true;
                LogMessage("Quit was handled through an EngineEvent");
                break;
            }
        }

        {
            std::unique_lock<std::mutex> lock(m_RenderMutex);
            for (const MeshRequest& meshRequest : m_MeshCreationRequests)
            {
                DynamicMesh dynamicMesh;
                dynamicMesh.LoadToGPU(meshRequest.m_PendingMesh);

                m_LoadedDynamicMeshes.emplace(std::make_pair(meshRequest.m_ID, dynamicMesh));
            }
            m_MeshCreationRequests.clear();

            for(const DynamicMeshID& meshID : m_MeshDestructionRequests)
            {
                DynamicMesh& dynamicMesh = m_LoadedDynamicMeshes[meshID];
                dynamicMesh.ReleaseFromGPU();

                m_LoadedDynamicMeshes.erase(meshID);
            }
            m_MeshDestructionRequests.clear();
        }

        m_ShaderLoader.HandleLoadRequests();
        m_TextureLoader.HandleLoadRequests();
        m_MeshLoader.HandleLoadRequests();

        if(!quitRequested)
        {
            std::unique_lock<std::mutex> lock(m_RenderMutex);
            m_StartRenderCV.wait(lock, [this](){return m_ReadyToRender;});

            m_ReadyToRender = false;
            Render(m_CurrentScenes, _window);
            Render(m_CurrentUIElements, _window);
            _window->display();

            m_RenderComplete = true;

            lock.unlock();
            m_StartRenderCV.notify_one();
        }
    }

    _window->close();
}

void SceneRenderer::HandleEvent(EngineEvent _event)
{
    m_EventHandler.PushEvent(_event);
}

void SceneRenderer::RequestRender(std::vector<Renderable::Scene> _scenes, std::vector<Renderable::DrawableVariant> _uiElements)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Main);

    std::unique_lock<std::mutex> lock(m_RenderMutex);

    if (!m_IsFirstFrame)
    {
        // Wait for previous frame to finish
        m_StartRenderCV.wait(lock, [this](){ return m_RenderComplete;});
    }

    m_IsFirstFrame = false;

    m_CurrentScenes = _scenes;
    m_CurrentUIElements = _uiElements;
    m_RenderComplete = false;
    m_ReadyToRender = true;

    lock.unlock();
    m_StartRenderCV.notify_one();
}

void SceneRenderer::RequestTerminate()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Main);

    // We are terminating, so the render thread won't receive another Render request,
    // but if it's already waiting for one we need to notify the thread one more time.

    std::unique_lock<std::mutex> lock(m_RenderMutex);
    m_RenderComplete = false;
    m_ReadyToRender = true;
    m_CurrentScenes.clear();
    m_CurrentUIElements.clear();
    lock.unlock();
    m_StartRenderCV.notify_one();
}

DynamicMeshID SceneRenderer::RequestDynamicMeshCreation(const RawMesh &_mesh)
{
    assert(m_NextAvailableID < std::numeric_limits<DynamicMeshID >::max());

    std::unique_lock<std::mutex> lock(m_RenderMutex);   // TODO Separate lock for dynamic requests?
    m_MeshCreationRequests.push_back({m_NextAvailableID, _mesh});

    return m_NextAvailableID++;
}

void SceneRenderer::RequestDynamicMeshDestruction(DynamicMeshID _meshID)
{
    std::unique_lock<std::mutex> lock(m_RenderMutex);   // TODO Separate lock for dynamic requests?
    m_MeshDestructionRequests.push_back(_meshID);
}

void SceneRenderer::Init()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    GLenum glewError = glewInit();

    if (glewError != GLEW_OK)
    {
        LogError("Failed to initialise glew.");
        return;
    }
    glEnable(GL_TEXTURE_2D);

    Globals::ReportGLError("glewInit");
    Globals::ReportGLProperties();
}

void SceneRenderer::Render(std::vector<Renderable::Scene> _scenes, std::shared_ptr<sf::RenderWindow> _window)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    ClearScreen();

    Renderable::RenderMode currentMode = Renderable::RenderMode::None;

    for (auto& scene : _scenes)
    {
        if (scene.m_RenderMode != currentMode)
        {
            switch (scene.m_RenderMode)
            {
            case Renderable::RenderMode::None:
                continue;
            case Renderable::RenderMode::Normal:
            case Renderable::RenderMode::Vista:
            {
                PrepareRender();
                glDepthFunc(GL_LESS);
                break;
            }
            case Renderable::RenderMode::Wireframe:
            {
                PrepareWireframeRender();
                glDepthFunc(GL_LESS);
                break;
            }
            case Renderable::RenderMode::Background:
            {
                PrepareRender();
                glDepthFunc(GL_LEQUAL);
                break;
            }
            default:
            {
                assert(false);
            }
            }
        }
        currentMode = scene.m_RenderMode;

        Render(scene, _window);

        if (currentMode == Renderable::RenderMode::Vista)
        {
            glClear(GL_DEPTH_BUFFER_BIT);
        }
    }

    PostRender();
}

void SceneRenderer::PrepareRender()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
}

void SceneRenderer::PrepareWireframeRender()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    glEnable(GL_DEPTH_TEST);

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SceneRenderer::ClearScreen()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void SceneRenderer::Render(Renderable::Scene& _scene, std::shared_ptr<sf::RenderWindow> _window)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    for (const auto& sceneObject : _scene.m_SceneObjects)
    {
        Renderable::Mesh mesh;

        if (sceneObject.m_MeshID != DYNAMICMESHID_INVALID)
        {
            mesh = m_LoadedDynamicMeshes.find(sceneObject.m_MeshID)->second.GetMesh();
        }
        else
        {
            mesh = sceneObject.m_Mesh;
        }

        glBindVertexArray(mesh.m_VaoHandle);
        Globals::ReportGLError("glBindVertexArray in renderer");

        ShaderProgram* shaderProgram = sceneObject.m_Shader.GetAsset();
        if (shaderProgram == nullptr)
        {
            LogError("ShaderProgram was not loaded.");
            continue;
        }

        shaderProgram->Use();

        for (const auto& attrib : mesh.m_VertexAttribs)
        {
            glEnableVertexAttribArray(attrib);
        }

        shaderProgram->SetUniformMat4("frplTransform", _scene.m_CameraTransform * sceneObject.m_Transform);
        shaderProgram->SetUniformFloat("frplAspectRatio", static_cast<f32>(_window->getSize().x) / static_cast<f32>(_window->getSize().y));


        shaderProgram->SetUniformMat4("frplModelTransform", sceneObject.m_Transform);

        glm::mat4 normalMatrix;
        if (_scene.m_RenderMode == Renderable::RenderMode::Background)
        {
            // Special case for the background, we need the camera's rotation for the normal, rather than the object.
            // We'll probably end up needing this anyway so it may become a different uniform
            normalMatrix = _scene.m_CameraRotation;
        }
        else
        {
            normalMatrix = MathsHelpers::GetRotationMatrix(sceneObject.m_Transform);
        }

        shaderProgram->SetUniformMat4("frplNormalTransform", normalMatrix);
        // TODO
        shaderProgram->SetUniformFloat3("frplBaseColor", glm::vec3());
        shaderProgram->SetUniformFloat3("frplCameraWorldPosition", _scene.m_CameraRelativePosition);

        shaderProgram->SetUniformFloat3("frplDirectionalLight.direction", _scene.m_DirectionalLight.m_Direction);
        shaderProgram->SetUniformFloat3("frplDirectionalLight.color", _scene.m_DirectionalLight.m_Color);
        shaderProgram->SetUniformFloat("frplDirectionalLight.intensity", _scene.m_DirectionalLight.m_Intensity);

        shaderProgram->SetUniformFloat3("frplAmbientLight.color", _scene.m_AmbientLight.m_Color);
        shaderProgram->SetUniformFloat("frplAmbientLight.intensity", _scene.m_AmbientLight.m_Intensity);

        Texture* texture = sceneObject.m_Texture.GetAsset();

        if (texture != nullptr)
        {
            switch (texture->GetTextureType())
            {
            case TextureAssetType::Image:
                texture->BindAsTexture(shaderProgram, 0);       // TODO Weird syntax, refactor
                break;
            case TextureAssetType::Cubemap:
                texture->BindAsCubemap(shaderProgram, 0);
                break;
            default:
                break;
            }
            static_assert(static_cast<u32>(TextureAssetType::Count) == 2);
        }

        glDrawElements(GL_TRIANGLES, mesh.m_SizeofElements, GL_UNSIGNED_INT, 0);

        if(texture != nullptr)
        {
            texture->Unbind();
        }

        //Unbind vertex array
        glBindVertexArray(0);
    }
}

void SceneRenderer::PostRender()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    // Cleanup the OpenGL state to allow for SFML rendering
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void SceneRenderer::Render(std::vector<Renderable::DrawableVariant> _uiElements, std::shared_ptr<sf::RenderWindow> _window)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    for(const Renderable::DrawableVariant& variant : _uiElements)
    {
        std::visit([_window](auto&& value){_window->draw(value);}, variant);
    }
}
