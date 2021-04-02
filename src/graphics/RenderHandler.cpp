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

#include "RenderHandler.h"
#include "src/graphics/DynamicMeshID.h"
#include "src/graphics/MeshType.h"
#include "src/graphics/Scene.h"
#include "src/world/particles/ParticleSystemComponent.h"

#include <memory>

#include <src/engine/EngineConfig.h>
#include <src/tools/MathsHelpers.h>
#include <src/world/FreelookCameraComponent.h>
#include <src/world/planet/PlanetGeneration.h>
#include <src/world/SpawningHandler.h>
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

    m_HACKTerrainVolumeTexture0 = AssetHandle<Texture>(TextureAsset_Volume_Perlin128);
    m_HACKTerrainVolumeTexture1 = AssetHandle<Texture>(TextureAsset_Volume_Grass128);

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
        m_HACKTerrainVolumeTexture0 = AssetHandle<Texture>();
        m_HACKTerrainVolumeTexture1 = AssetHandle<Texture>();
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
    case EngineEvent::Type::RenderMaxTerrainLOD:
    {
        m_MaxTerrainLOD = _event.GetIntData();
        break;
    }
    case EngineEvent::Type::RenderSetParticles:
    {
        m_ShouldRenderParticles = static_cast<bool>(_event.GetIntData());
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

    Renderable::Frame frame;

    if (_world != nullptr)
    {
        const FreelookCameraComponent* camera = _world->GetPlayerHandler()->GetLocalCamera();

        UpdateSharedDynamicMeshes(_world, frame);

        if (camera != nullptr)
        {
            GenerateBackgroundScene(_world, camera, frame.m_PendingScenes);
            GenerateScenes(_world, camera, frame);

            GenerateBoundingBoxScenes(_world, camera, _uiDisplay->m_Debug3DDrawingQueue, frame.m_PendingScenes);
        }
    }

    STL::Append(frame.m_MeshDestructionRequests, m_ExtraMeshDesructionRequests);
    m_ExtraMeshDesructionRequests.clear();

    if (_uiDisplay != nullptr)
    {
        frame.m_PendingUIElements = _uiDisplay->m_DrawingQueue;
    }

    {
        ProfileCurrentScope("Wait for GPU");
        m_SceneRenderer.RequestRender(frame);
    }
}

void RenderHandler::GenerateScenes(const World* _world, const FreelookCameraComponent* _camera, Renderable::Frame& _inOutFrame)
{
    ProfileCurrentFunction();

    Renderable::Scene sceneToRender;

    GenerateSceneCamera(_world, _camera, sceneToRender);
    GenerateSceneGlobalLighting(_world, _camera, sceneToRender);

    std::vector<Renderable::InstancedSceneObject> particleInstances;
    std::vector<Renderable::InstancedSceneObject> meshInstances;
    
    for (const WorldZone& zone : _world->GetActiveZones())
    {
        sceneToRender.m_RenderMode = m_ShouldRenderWireframe ? Renderable::RenderMode::Wireframe
                                                             : Renderable::RenderMode::Normal;

        // Offset the view to account for zone.
        const glm::mat4 viewTransform = zone.GetRelativeTransform(_camera->GetOwnerObject()->GetRef().m_ZoneCoordinates);
        
        for (const RenderComponent& component : zone.GetComponents<RenderComponent>())
        {
            if (component.m_CanInstance)
            {
                auto it = std::find_if(meshInstances.begin(), meshInstances.end(), [&component](const Renderable::InstancedSceneObject& instance)
                                                                                 {
                                                                                     // TODO We should have a smarter way to do this.
                                                                                     return component.GetShader() == instance.m_Solid.m_Material.m_Shader
                                                                                         && component.GetTexture() == instance.m_Solid.m_Material.m_Textures[0].second
                                                                                         && component.m_DynamicID == instance.m_Solid.m_MeshID;
                                                                                 });
                Renderable::InstancedSceneObject* instance = nullptr;
                
                if (it == meshInstances.end())
                {
                    instance = &meshInstances.emplace_back();

                    if (component.m_DynamicID != DYNAMICMESHID_INVALID)
                    {
                        instance->m_Solid.m_MeshID = component.m_DynamicID;
                    }
                    else
                    {
                        LogError("DynamicMesh wasn't loaded for instanced RenderComponent.");
                    }
                                
                    instance->m_Solid.m_Material.m_Shader = component.GetShader();
                    instance->m_Solid.m_Material.m_Textures.emplace_back("tex2D_0", component.GetTexture());
                    instance->m_Solid.m_MeshType = component.GetMeshType();
                }
                else
                {
                    instance = &(*it);
                }

                instance->m_Solid.m_NeedsDepthSort = false;

                const WorldObject* worldObject = component.GetOwnerObject();
                assert(worldObject != nullptr);
                
                instance->m_Transforms.push_back(viewTransform * worldObject->GetZoneTransform());
            }
            else
            {
                const WorldObject* worldObject = component.GetOwnerObject();
                assert(worldObject != nullptr);
     
                Renderable::SceneObject sceneObject;
                sceneObject.m_Transform = viewTransform * worldObject->GetZoneTransform();
                sceneObject.m_Solid.m_Material.m_Shader = component.GetShader();
                sceneObject.m_Solid.m_Material.m_Textures.emplace_back("tex2D_0",  component.GetTexture());
                sceneObject.m_Solid.m_MeshType = component.GetMeshType();
     
                const StaticMesh* mesh = component.GetMesh().GetAsset();
                if (mesh != nullptr)
                {
                    sceneObject.m_Solid.m_Mesh = mesh->GetMesh();
                }
                else if (component.m_DynamicID != DYNAMICMESHID_INVALID)
                {
                    sceneObject.m_Solid.m_MeshID = component.m_DynamicID;
                }
                else
                {
                    LogError("StaticMesh wasn't loaded, and no dynamic ID was specified.");
                }
     
                sceneToRender.m_SceneObjects.push_back(sceneObject);
            }
        }

        for (const LightComponent& component : zone.GetComponents<LightComponent>())
        {
            Renderable::PointLight light;
            light.m_Color = component.m_Emitter.m_Color;
            light.m_Intensity = component.m_Emitter.m_Brightness;

            const WorldObject* worldObject = component.GetOwnerObject();
            assert(worldObject != nullptr);

            light.m_Origin = worldObject->GetPosition() + component.m_Emitter.m_LocalOffset + MathsHelpers::GetPosition(viewTransform);

            sceneToRender.m_PointLights.push_back(light);
        }

        if (m_ShouldRenderParticles)
        {
            for (const ParticleSystemComponent& component : zone.GetComponents<ParticleSystemComponent>())
            {
                glm::mat4 objectTransform = viewTransform * component.GetOwnerObject()->GetZoneTransform();
                
                for (const ParticleEmitter& emitter : component.m_ParticleSystem.m_Emitters)
                {
                    auto it = std::find_if(particleInstances.begin(), particleInstances.end(), [&emitter](const Renderable::InstancedSceneObject& instance)
                                                                                 {
                                                                                     // TODO We should have a smarter way to do this.
                                                                                     return emitter.m_Shader == instance.m_Solid.m_Material.m_Shader
                                                                                         && emitter.m_Texture == instance.m_Solid.m_Material.m_Textures[0].second
                                                                                         && emitter.m_Blending == instance.m_Solid.m_Blending;
                                                                                 });
                    Renderable::InstancedSceneObject* instance = nullptr;
                    
                    if (it == particleInstances.end())
                    {
                        instance = &particleInstances.emplace_back();

                        if (emitter.m_MeshID != DYNAMICMESHID_INVALID)
                        {
                            instance->m_Solid.m_MeshID = emitter.m_MeshID;
                        }
                        else
                        {
                            LogError("DynamicMesh wasn't loaded for particle emitter.");
                        }
                                    
                        instance->m_Solid.m_Material.m_Shader = emitter.m_Shader;
                        instance->m_Solid.m_Material.m_Textures.emplace_back("tex2D_0", emitter.m_Texture);
                        instance->m_Solid.m_MeshType = MeshType::Billboard;
                        instance->m_Solid.m_Blending = emitter.m_Blending;

                        if (emitter.m_NeedsDepthSort)
                        {
                            instance->m_SortOrigin = MathsHelpers::GetPosition(component.GetOwnerObject()->GetZoneTransform()) + emitter.m_RelativePosition;
                        }
                    }
                    else
                    {
                        instance = &(*it);
                    }

                    instance->m_Solid.m_NeedsDepthSort = emitter.m_NeedsDepthSort;
                    
                    for (const Particle& particle : emitter.m_Particles)
                    {
                        const glm::mat4 translation = glm::translate(objectTransform, emitter.m_RelativePosition + particle.m_RelativePosition + particle.m_OffsetPosition);

                        if (emitter.m_NeedsDepthSort)
                        {
                            Renderable::SceneObjectInstanceData data;
                            data.m_Transform = translation * particle.m_Rotation;
                            data.m_Color = particle.m_Color;
                            instance->m_InstanceDataForSorting.push_back(data);
                        }
                        else
                        {
                            instance->m_Transforms.push_back(translation * particle.m_Rotation);
                            
                            if (emitter.m_UseParticleColor)
                            {
                                instance->m_Colors.push_back(particle.m_Color);
                            }
                        }
                    }
                }
            }
        }
        
        UpdateDynamicMesh(zone.GetTerrainComponent().m_DynamicMesh, viewTransform * zone.GetTerrainModelTransform(),
                          m_HACKTerrainShader, sceneToRender.m_SceneObjects, _inOutFrame, 0);

        if (zone.ContainsPlayer())
        {
            m_DebugZoneRawMeshForDebugDraw = zone.GetTerrainComponent().m_DynamicMesh.m_RawMesh;
        }
    }

    if (m_ShouldRenderVista)
    {
        sceneToRender.m_RenderMode = m_ShouldRenderWireframe ? Renderable::RenderMode::Wireframe
                                                             : Renderable::RenderMode::Vista;

        UpdateDynamicMesh(_world->GetVistaHandler()->m_DynamicMesh,
                          _world->GetVistaHandler()->GetTerrainModelTransform(), m_HACKTerrainShader,
                          sceneToRender.m_SceneObjects, _inOutFrame, m_MaxTerrainLOD);
    }

    STL::Append(sceneToRender.m_Instances, meshInstances);
    
    {
        ProfileCurrentScope("Depth sort");
        
        const glm::vec3 cameraPosition = _camera->GetOwnerObject()->GetPosition();
        const glm::vec3 cameraDirection = _camera->GetLookDirection();
        for (Renderable::InstancedSceneObject& instance : particleInstances)
        {
            if (instance.m_Solid.m_NeedsDepthSort)
            {                
                std::sort(instance.m_InstanceDataForSorting.begin(),
                          instance.m_InstanceDataForSorting.end(),
                          [&cameraPosition, &cameraDirection](const Renderable::SceneObjectInstanceData& _lhs, const Renderable::SceneObjectInstanceData& _rhs)
                                                             {
                                                                 // @Performance Should calculate the values once only and cache.
                                                                 const f32 distA = glm::dot(MathsHelpers::GetPosition(_lhs.m_Transform) - cameraPosition, cameraDirection);
                                                                 const f32 distB = glm::dot(MathsHelpers::GetPosition(_rhs.m_Transform) - cameraPosition, cameraDirection);
                                                                 return distA > distB;
                                                             });

                // Convert back into separate arrays for drawing.
                for (const Renderable::SceneObjectInstanceData& data : instance.m_InstanceDataForSorting)
                {
                    instance.m_Transforms.push_back(data.m_Transform);
                    instance.m_Colors.push_back(data.m_Color);
                }

                // Avoid copying this redundant data.
                instance.m_InstanceDataForSorting.clear();
            }
        }
        
        std::sort(particleInstances.begin(), particleInstances.end(),
                  [&cameraPosition, &cameraDirection](const Renderable::InstancedSceneObject& _lhs, const Renderable::InstancedSceneObject& _rhs)
                                                             {
                                                                 if (!_lhs.m_SortOrigin.has_value() && !_rhs.m_SortOrigin.has_value())
                                                                 {
                                                                     return false;
                                                                 }

                                                                 if (_lhs.m_SortOrigin.has_value() && !_rhs.m_SortOrigin.has_value())
                                                                 {
                                                                     return false;
                                                                 }

                                                                 if (!_lhs.m_SortOrigin.has_value() && _rhs.m_SortOrigin.has_value())
                                                                 {
                                                                     return true;
                                                                 }                                                                 
                                                                 // @Performance Should calculate the values once only and cache.
                                                                 const f32 distA = glm::dot(*_lhs.m_SortOrigin - cameraPosition, cameraDirection);
                                                                 const f32 distB = glm::dot(*_rhs.m_SortOrigin - cameraPosition, cameraDirection);
                                                                 return distA > distB;
                                                             });
                  
        
        STL::Append(sceneToRender.m_Instances, particleInstances);    
    }
    
    _inOutFrame.m_PendingScenes.push_back(sceneToRender);
}

void RenderHandler::UpdateSharedDynamicMeshes(const World* _world, Renderable::Frame& _inOutFrame)
{
    ProfileCurrentFunction();
    
    const SpawningHandler* spawningHandler = _world->GetSpawningHandler();

    if (spawningHandler != nullptr)
    {
        for (DynamicMeshHandle& handle : spawningHandler->m_SpawnedPlantMeshes)
        {
            // TODO Factor this out to be more shared with how the terrain uses DynamicMeshes.
            if (handle.m_DynamicMeshId == DYNAMICMESHID_INVALID && !handle.GetRawMesh().IsEmpty())
            {
                Renderable::MeshRequest& request = _inOutFrame.m_MeshCreationRequests.emplace_back();
                request.m_ID = m_NextAvailableDynamicMeshID++;
                request.m_PendingMesh = handle.m_RawMesh;

                handle.m_DynamicMeshId = request.m_ID;
            }
        }

        // TODO Factor this out to be more shared with how the terrain uses DynamicMeshes.
        if (spawningHandler->m_ParticleSystemMesh.m_DynamicMeshId == DYNAMICMESHID_INVALID && !spawningHandler->m_ParticleSystemMesh.GetRawMesh().IsEmpty())
        {
            Renderable::MeshRequest& request = _inOutFrame.m_MeshCreationRequests.emplace_back();
            request.m_ID = m_NextAvailableDynamicMeshID++;
            request.m_PendingMesh = spawningHandler->m_ParticleSystemMesh.m_RawMesh;
       
            spawningHandler->m_ParticleSystemMesh.m_DynamicMeshId = request.m_ID;
        }
    }
}

void RenderHandler::UpdateDynamicMesh(DynamicMeshHandle& _handle, const glm::mat4& _transform,
                                      const AssetHandle<ShaderProgram>& _shader,
                                      std::vector<Renderable::SceneObject>& _outSceneObjects,
                                      Renderable::Frame& _inOutFrame,
                                      u32 _terrainLOD)
{
    ProfileCurrentFunction();
    
    Renderable::SceneObject sceneObject;
    sceneObject.m_Transform = _transform;
    sceneObject.m_Solid.m_Material.m_Shader = _shader; // TODO Somewhere to source this for DynamicMeshes

    // TODO Support multiple textures, have somewhere to source them.
    sceneObject.m_Solid.m_Material.m_Textures.emplace_back("texPerlin", m_HACKTerrainVolumeTexture0);
    sceneObject.m_Solid.m_Material.m_Textures.emplace_back("texGrass", m_HACKTerrainVolumeTexture1);

    // TODO a more generic source for this.
    // We should use ifdefs to make this a compile-time switch (e.g. multiple versions of the
    // shader with different switches applied). Can also avoid binding the detail textures for
    // these lower LOD shaders.
    sceneObject.m_Solid.m_Material.m_IntUniforms.emplace_back("frplTerrainLod", _terrainLOD);

    if (_handle.m_DynamicMeshId == DYNAMICMESHID_INVALID)
    {
        // The GPU DynamicMesh needs updating.

        if (_handle.m_PreviousDynamicMeshId != DYNAMICMESHID_INVALID)
        {
            // This frame, we will render the previous version of the mesh while we request a new one.
            sceneObject.m_Solid.m_MeshID = _handle.m_PreviousDynamicMeshId;

            _outSceneObjects.push_back(sceneObject);
        }

        // Make the request for a new GPU mesh.
        if (!_handle.m_RawMesh.IsEmpty())
        {
            Renderable::MeshRequest& request = _inOutFrame.m_MeshCreationRequests.emplace_back();
            request.m_ID = m_NextAvailableDynamicMeshID++;
            request.m_PendingMesh = _handle.m_RawMesh;

            _handle.m_DynamicMeshId = request.m_ID;
        }
    }
    else
    {
        // The mesh is up-to-date, just need to render it.

        if (_handle.m_PreviousDynamicMeshId != DYNAMICMESHID_INVALID)
        {
            // We don't need the old mesh any more, we can clean it up.
            _inOutFrame.m_MeshDestructionRequests.push_back(_handle.m_PreviousDynamicMeshId);
            _handle.m_PreviousDynamicMeshId = DYNAMICMESHID_INVALID;
        }

        sceneObject.m_Solid.m_MeshID = _handle.m_DynamicMeshId;
    }

    _outSceneObjects.push_back(sceneObject);
}

void RenderHandler::OnDynamicMeshDestroyed(DynamicMeshID _id)
{
    if (_id != DYNAMICMESHID_INVALID)
    {
        m_ExtraMeshDesructionRequests.push_back(_id);
    }
}

void RenderHandler::GenerateBoundingBoxScenes(const World* _world, const FreelookCameraComponent* _camera,
                                              const std::vector<ZoneDebugRenderable>& _debugRenderables,
                                              std::vector<Renderable::Scene>& _scenes)
{
    ProfileCurrentFunction();

    const StaticMesh* cubeMesh = m_UnitCube.GetAsset();
    const StaticMesh* sphereMesh = m_UnitSphere.GetAsset();
    const StaticMesh* cylinderMesh = m_UnitCylinder.GetAsset();

    for (const WorldZone& zone : _world->GetActiveZones())
    {
        Renderable::Scene sceneToRender;
        sceneToRender.m_RenderMode = Renderable::RenderMode::Wireframe;

        GenerateSceneCamera(_world, _camera, sceneToRender);

        // Offset the view to account for zone.
        sceneToRender.m_ViewTransform *= zone.GetRelativeTransform(_camera->GetOwnerObject()->GetWorldPosition().m_ZoneCoordinates);

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
            sceneObject.m_Solid.m_Material.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

            if (cubeMesh != nullptr)
            {
                sceneObject.m_Solid.m_Mesh = cubeMesh->GetMesh();
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
                            sceneObject.m_Solid.m_Material.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

                            if (cubeMesh != nullptr)
                            {
                                sceneObject.m_Solid.m_Mesh = cubeMesh->GetMesh();
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
                            * MathsHelpers::GenerateRotationMatrixFromRight(normal);
                    sceneObject.m_Solid.m_Material.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

                    if (arrowMesh != nullptr)
                    {
                        sceneObject.m_Solid.m_Mesh = arrowMesh->GetMesh();
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

void RenderHandler::AddBoundingBoxObject(const glm::mat4 _transform, const StaticMesh* _mesh, Renderable::Scene& _outScene) const
{
    Renderable::SceneObject sceneObject;
    sceneObject.m_Transform = _transform;
    sceneObject.m_Solid.m_Material.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Unlit_Untextured);

    if (_mesh != nullptr)
    {
        sceneObject.m_Solid.m_Mesh = _mesh->GetMesh();
    }
    else
    {
        LogError("StaticMesh wasn't loaded or was unavailable for the CollisionPrimitiveType.");
    }
    _outScene.m_SceneObjects.push_back(sceneObject);
}

void RenderHandler::GenerateBackgroundScene(const World* _world, const FreelookCameraComponent* _camera,
                                            std::vector<Renderable::Scene>& _scenes) const
{
    ProfileCurrentFunction();

    const StaticMesh* quadMesh = m_Quad.GetAsset();

    Renderable::Scene sceneToRender;
    sceneToRender.m_RenderMode = Renderable::RenderMode::Background;

    sceneToRender.m_ViewTransform = glm::mat4(1.f);
    sceneToRender.m_ProjectionTransform = glm::mat4(1.f);

    sceneToRender.m_CameraInverseProjection = glm::inverse(glm::perspective(m_DefaultFov,
                                                               static_cast<f32>(m_WindowResolution.x) / static_cast<f32>(m_WindowResolution.y),
                                                                            0.1f,
                                                                            _camera->GetFarClipDistance()) * glm::inverse(MathsHelpers::GetRotationMatrix(_camera->GetCameraZoneTransform())));

    GenerateSceneGlobalLighting(_world, _camera, sceneToRender);

    const Planet* planet = _world->GetPlanet();

    if (planet != nullptr)
    {
        // TODO clean up this calculation.
        sceneToRender.m_Atmosphere.m_Origin = WorldPosition(planet->m_OriginZone, glm::vec3(0.f)).GetPositionRelativeTo(_camera->GetOwnerObject()->GetRef().m_ZoneCoordinates)
                                              - _camera->GetOwnerObject()->GetPosition();
        sceneToRender.m_Atmosphere.m_GroundRadius = planet->m_Radius;
        sceneToRender.m_Atmosphere.m_AtmosphereHeight = planet->m_AtmosphereHeight;
        sceneToRender.m_Atmosphere.m_AtmosphereBlendOutHeight = planet->m_BlendOutHeight;

        sceneToRender.m_LocalUpDirection = PlanetGeneration::GetUpDirection(*planet, _camera->GetOwnerObject()->GetWorldPosition());
    }

    Renderable::SceneObject sceneObject;
    sceneObject.m_Transform = glm::mat4(1.f);
    if (quadMesh != nullptr)
    {
        sceneObject.m_Solid.m_Mesh = quadMesh->GetMesh();
    }
    sceneObject.m_Solid.m_Material.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Skybox);

    sceneToRender.m_SceneObjects.emplace_back(sceneObject);

    _scenes.push_back(sceneToRender);
}

void RenderHandler::GenerateSceneCamera(const World* _world, const FreelookCameraComponent* _camera, Renderable::Scene& _outScene) const
{
    glm::mat4 view = glm::inverse(_camera->GetCameraZoneTransform());
    glm::mat4 projection = glm::perspective(m_DefaultFov,
                                            static_cast<f32>(m_WindowResolution.x) /
                                            static_cast<f32>(m_WindowResolution.y),
                                            0.1f,
                                            _camera->GetFarClipDistance());

    _outScene.m_ProjectionTransform = projection;
    _outScene.m_ViewTransform = view;
    _outScene.m_CameraRelativePosition = MathsHelpers::GetPosition(_camera->GetCameraZoneTransform());
}

void RenderHandler::GenerateSceneGlobalLighting(const World* _world, const FreelookCameraComponent* _camera, Renderable::Scene& _outScene) const
{
    const Planet* planet = _world->GetPlanet();

    _outScene.m_DirectionalLight.m_Direction = _world->GetEnvironmentState().GetSunDirection();
    _outScene.m_DirectionalLight.m_Color = _world->GetEnvironmentState().GetSunColor();
    _outScene.m_DirectionalLight.m_Intensity = _world->GetEnvironmentState().GetSunIntensity(planet, _camera->GetOwnerObject()->GetWorldPosition());

    _outScene.m_AmbientLight.m_Color = _world->GetEnvironmentState().GetAmbientColor();
    _outScene.m_AmbientLight.m_Intensity = _world->GetEnvironmentState().GetAmbientIntensity();

    if (planet != nullptr)
    {
        _outScene.m_LocalUpDirection = PlanetGeneration::GetUpDirection(*planet, _camera->GetOwnerObject()->GetWorldPosition());
    }
}
