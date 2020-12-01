//
// Created by alastair on 05/10/18.
//

#include "src/graphics/Scene.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <src/tools/MathsHelpers.h>
#include <src/graphics/SceneRenderer.h>
#include <src/graphics/ShaderProgram.h>
#include <src/graphics/Texture.h>
#include <src/graphics/ui/SFMLDraw.h>

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

        m_ShaderLoader.HandleLoadRequests();
        m_TextureLoader.HandleLoadRequests();
        m_MeshLoader.HandleLoadRequests();
        m_UITextureLoader.HandleLoadRequests();

        if(!quitRequested)
        {
            std::unique_lock<std::mutex> lock(m_RenderMutex);
            m_StartRenderCV.wait(lock, [this](){return m_ReadyToRender;});

            m_ReadyToRender = false;
            HandleDynamicMeshRequests();
            Render(m_CurrentScenes, _window);
            Render(m_CurrentUIElements, _window);
            _window->display();

            m_RenderComplete = true;

            lock.unlock();
            m_StartRenderCV.notify_one();
        }
    }

    m_ShaderLoader.Shutdown();
    m_TextureLoader.Shutdown();
    m_MeshLoader.Shutdown();
    m_UITextureLoader.Shutdown();
}

void SceneRenderer::HandleEvent(EngineEvent _event)
{
    m_EventHandler.PushEvent(_event);
}

void SceneRenderer::RequestRender(Renderable::Frame _frame)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Main);

    std::unique_lock<std::mutex> lock(m_RenderMutex);

    if (!m_IsFirstFrame)
    {
        // Wait for previous frame to finish
        m_StartRenderCV.wait(lock, [this](){ return m_RenderComplete;});
    }

    m_IsFirstFrame = false;

    m_CurrentScenes = _frame.m_PendingScenes;
    m_CurrentUIElements = _frame.m_PendingUIElements;
    m_MeshCreationRequests = _frame.m_MeshCreationRequests;
    m_MeshDestructionRequests = _frame.m_MeshDestructionRequests;
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

    GLHelpers::ReportError("glewInit");
    GLHelpers::ReportProperties();
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

    for (const Renderable::SceneObject& sceneObject : _scene.m_SceneObjects)
    {
        const Renderable::Mesh* mesh = BindMesh(sceneObject.m_Solid);

        if (mesh == nullptr)
        {
            continue;
        }

        ShaderProgram* shaderProgram = UseShaderWithMesh(sceneObject.m_Solid, mesh);

        if (shaderProgram == nullptr)
        {
            continue;
        }
        
        shaderProgram->SetUniformMat4("frplTransform", ComputeTransform(_scene, sceneObject.m_Solid, sceneObject.m_Transform));
        shaderProgram->SetUniformMat4("frplModelTransform", sceneObject.m_Transform);

        shaderProgram->SetUniformMat4("frplNormalTransform", MathsHelpers::GetRotationMatrix(sceneObject.m_Transform));
        // TODO
        shaderProgram->SetUniformFloat3("frplBaseColor", sceneObject.m_Solid.m_BaseColor);

        SetSceneShaderParameters(_scene, shaderProgram, _window);
        SetMaterialShaderParameters(shaderProgram, sceneObject.m_Solid);
        SetupTextures(shaderProgram, sceneObject.m_Solid);
        
        glDrawElements(GL_TRIANGLES, mesh->m_NumberOfElements, GL_UNSIGNED_INT, nullptr);

        UnbindTextures(shaderProgram, sceneObject.m_Solid);

        UnbindMesh(mesh);
    }

    for (const Renderable::InstancedSceneObject& instancedSceneObject : _scene.m_Instances)
    {
        const Renderable::Mesh* mesh = BindMesh(instancedSceneObject.m_Solid);

        if (mesh == nullptr)
        {
            continue;
        }
        
        ShaderProgram* shaderProgram = UseShaderWithMesh(instancedSceneObject.m_Solid, mesh);

        if (shaderProgram == nullptr)
        {
            continue;
        }
        
        std::vector<glm::mat4> instanceTransforms;
        std::vector<glm::mat4> instanceNormalTransforms;
        
        for (const glm::mat4& transform : instancedSceneObject.m_Transforms)
        {
            instanceTransforms.push_back(ComputeTransform(_scene, instancedSceneObject.m_Solid, transform));
            instanceNormalTransforms.push_back(MathsHelpers::GetRotationMatrix(transform));
        }

        const GLint instanceTransformLocation = glGetAttribLocation(shaderProgram->GetProgramHandle(), "frplInstanceTransform");

        if (instanceTransformLocation < 0)
        {
            LogError("Couldn't get location for instance transforms");
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, instanceTransformLocation);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instanceTransforms.size(), instanceTransforms.data(), GL_DYNAMIC_DRAW);

        const GLint instanceNormalTransformLocation = glGetAttribLocation(shaderProgram->GetProgramHandle(), "frplInstanceNormalTransform");

        if (instanceNormalTransformLocation < 0)
        {
            LogError("Couldn't get location for instance normal transforms");
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, instanceNormalTransformLocation);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instanceNormalTransforms.size(), instanceNormalTransforms.data(), GL_DYNAMIC_DRAW);

        
        SetSceneShaderParameters(_scene, shaderProgram, _window);
        SetMaterialShaderParameters(shaderProgram, instancedSceneObject.m_Solid);
        SetupTextures(shaderProgram, instancedSceneObject.m_Solid);

        glDrawElementsInstanced(GL_TRIANGLES, mesh->m_NumberOfElements, GL_UNSIGNED_INT, nullptr, instanceTransforms.size());
        GLHelpers::ReportError("glDrawElementsInstanced");
    
        UnbindTextures(shaderProgram, instancedSceneObject.m_Solid);

        UnbindMesh(mesh);
    }
}

const Renderable::Mesh* SceneRenderer::BindMesh(const Renderable::Solid& _solid) const
{
    const Renderable::Mesh* mesh;

    if (_solid.m_MeshID != DYNAMICMESHID_INVALID)
    {
        const auto dynamicMeshIt = m_LoadedDynamicMeshes.find(_solid.m_MeshID);

        if (dynamicMeshIt != m_LoadedDynamicMeshes.end())
        {
            mesh = &dynamicMeshIt->second.GetMesh();
        }
        else
        {
            LogError("Tried to render a DynamicMesh that is not loaded.");
            assert(false);
            return nullptr;
        }
    }
    else
    {
        mesh = &_solid.m_Mesh;
    }

    glBindVertexArray(mesh->m_VaoHandle);
    GLHelpers::ReportError("glBindVertexArray in renderer");

    return mesh;
}

void SceneRenderer::UnbindMesh(const Renderable::Mesh* _mesh) const
{
    // Unbind vertex array
    glBindVertexArray(0);
}

ShaderProgram* SceneRenderer::UseShaderWithMesh(const Renderable::Solid& _solid, const Renderable::Mesh* _mesh) const
{
    ShaderProgram* shaderProgram = _solid.m_Material.m_Shader.GetAsset();

    if (shaderProgram == nullptr)
    {
        LogError("ShaderProgram was not loaded.");
        return nullptr;
    }

    shaderProgram->Use();

    for (const auto& attrib : _mesh->m_VertexAttribs)
    {
        glEnableVertexAttribArray(attrib);
    }

    return shaderProgram;
}

glm::mat4 SceneRenderer::ComputeTransform(const Renderable::Scene& _scene, const Renderable::Solid& _solid, const glm::mat4& _objectTransform) const
{
    switch (_solid.m_MeshType)
    {
    case MeshType::Normal:
    {
        return _scene.m_ProjectionTransform * _scene.m_ViewTransform * _objectTransform;
    }
    case MeshType::Billboard:
    {
        glm::mat4 mvMatrix = _scene.m_ViewTransform * _objectTransform;
        MathsHelpers::SetRotationPart(mvMatrix, glm::mat3(1.f));

        return _scene.m_ProjectionTransform * mvMatrix;
    }
    case MeshType::OrientedBillboard:
    {
        glm::mat4 mvMatrix = _scene.m_ViewTransform * _objectTransform;

        // Clear x and z rotation only.
        mvMatrix[0][0] = 1.f;
        mvMatrix[0][1] = 0.f;
        mvMatrix[0][2] = 0.f;

        mvMatrix[2][0] = 0.f;
        mvMatrix[2][1] = 0.f;
        mvMatrix[2][2] = 1.f;

        return _scene.m_ProjectionTransform * mvMatrix;
    }
    default:
        break;
    }

    assert(false);
    return glm::mat4(1.f);
}

void SceneRenderer::SetSceneShaderParameters(const Renderable::Scene& _scene, ShaderProgram* _shaderProgram, const std::shared_ptr<const sf::RenderWindow> _window) const
{
    _shaderProgram->SetUniformMat4("frplCameraInverseProjection", _scene.m_CameraInverseProjection);
    _shaderProgram->SetUniformFloat("frplAspectRatio", static_cast<f32>(_window->getSize().x) / static_cast<f32>(_window->getSize().y));

    _shaderProgram->SetUniformFloat3("frplCameraWorldPosition", _scene.m_CameraRelativePosition);

    // TODO Should that really be per scene?
    _shaderProgram->SetUniformFloat3("frplLocalUpDirection", _scene.m_LocalUpDirection);
     
    _shaderProgram->SetUniformFloat3("frplDirectionalLight.direction", _scene.m_DirectionalLight.m_Direction);
    _shaderProgram->SetUniformFloat3("frplDirectionalLight.color", _scene.m_DirectionalLight.m_Color);
    _shaderProgram->SetUniformFloat("frplDirectionalLight.intensity", _scene.m_DirectionalLight.m_Intensity);
     
    _shaderProgram->SetUniformFloat3("frplAmbientLight.color", _scene.m_AmbientLight.m_Color);
    _shaderProgram->SetUniformFloat("frplAmbientLight.intensity", _scene.m_AmbientLight.m_Intensity);

    // TODO Pick the n nearest lights to the camera.
    // TODO Lights shared across zones/scenes.
    // Can set only once per frame?
    for (u32 lightIdx = 0; lightIdx < 8; ++lightIdx)
    {
        const std::string lightName = "frplLights[" + std::to_string(lightIdx) + "]";

        if (_scene.m_PointLights.size() <= lightIdx)
        {
            _shaderProgram->SetUniformFloat3(lightName + ".position", glm::vec3(0.f));
            _shaderProgram->SetUniformFloat3(lightName + ".color", Color(0.f));
            _shaderProgram->SetUniformFloat(lightName + ".intensity", 0.f);
            continue;
        }

        _shaderProgram->SetUniformFloat3(lightName + ".position", _scene.m_PointLights[lightIdx].m_Origin);
        _shaderProgram->SetUniformFloat3(lightName + ".color", _scene.m_PointLights[lightIdx].m_Color);
        _shaderProgram->SetUniformFloat(lightName + ".intensity", _scene.m_PointLights[lightIdx].m_Intensity);
    }

    _shaderProgram->SetUniformFloat3("frplAtmosphere.origin", _scene.m_Atmosphere.m_Origin);
    _shaderProgram->SetUniformFloat("frplAtmosphere.groundRadius", _scene.m_Atmosphere.m_GroundRadius);
    _shaderProgram->SetUniformFloat("frplAtmosphere.height", _scene.m_Atmosphere.m_AtmosphereHeight);
    _shaderProgram->SetUniformFloat("frplAtmosphere.blendOutHeight", _scene.m_Atmosphere.m_AtmosphereBlendOutHeight);
}

void SceneRenderer::SetupTextures(ShaderProgram* _shaderProgram, const Renderable::Solid& _solid) const
{
    u32 textureUnitIdx = 0;
     for (const std::pair<std::string, AssetHandle<Texture>>& texturePair : _solid.m_Material.m_Textures)
     {
         Texture* texture = texturePair.second.GetAsset();

         if (texture != nullptr)
         {
             texture->Bind(_shaderProgram, texturePair.first, textureUnitIdx);

             switch (_solid.m_MeshType)
             {
                 case MeshType::Normal:
                 {
                     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                     break;
                 }
                 case MeshType::Billboard:
                     [[fallthrough]];
                 case MeshType::OrientedBillboard:
                 {
                     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                     break;
                 }
             }
             ++textureUnitIdx;
         }
     }
}

void SceneRenderer::UnbindTextures(ShaderProgram* _shaderProgram, const Renderable::Solid& _solid) const
{
    for (s32 texIdx = _solid.m_Material.m_Textures.size() - 1; texIdx >= 0; --texIdx)
    {
        Texture* texture = _solid.m_Material.m_Textures[texIdx].second.GetAsset();

        if (texture != nullptr)
        {
            texture->Unbind(texIdx);
        }
    }
}

void SceneRenderer::SetMaterialShaderParameters(ShaderProgram* _shaderProgram, const Renderable::Solid& _solid) const
{
    for (const std::pair<std::string, u32>& uniform : _solid.m_Material.m_IntUniforms)
    {
        _shaderProgram->SetUniformInt(uniform.first, uniform.second);
    }

    _shaderProgram->SetUniformFloat4("frplBaseColor", _solid.m_BaseColor);
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

void SceneRenderer::HandleDynamicMeshRequests()
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    for (const Renderable::MeshRequest& meshRequest : m_MeshCreationRequests)
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

void SceneRenderer::Render(std::vector<Renderable::DrawableVariant> _uiElements, std::shared_ptr<sf::RenderWindow> _window)
{
    assert(ThreadUtils::tl_ThreadType == ThreadType::Render);

    _window->resetGLStates();
    for(const Renderable::DrawableVariant& variant : _uiElements)
    {
        std::visit([_window](auto&& value){SFMLDraw::Draw(value, _window);}, variant);
    }
}
