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

#include "FloraGeneration.h"
#include "src/assets/MeshAssets.h"
#include "src/assets/ShaderAssets.h"
#include "src/assets/TextureAssets.h"
#include "src/engine/AssetHandle.h"
#include "src/graphics/MeshImport.h"
#include "src/graphics/StaticMesh.h"
#include "src/world/particles/ParticleSystemComponent.h"

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <random>

#include <src/tools/MathsHelpers.h>

PlantInstance FloraGeneration::GeneratePlant(const FloraGenerationParams& _params, u32 _seed)
{
    PlantInstance result;

    // Must reserve enough nodes to avoid reallocation during the growth algorithm.
    result.m_Nodes.reserve(glm::pow(_params.m_MaxBranchingFactor, _params.m_MaxBranchingDepth) + 2);
    
    std::mt19937 gen(_seed);
    std::uniform_real_distribution<> unsignedDistribution(0.f, 1.f);
    
    {
        PlantInstanceNode node;
        node.m_RelativePosition = glm::vec3(0.f);
        node.m_Children[0] = 1;
        node.m_ChildCount = 1;

        node.m_Depth = 0;
        
        result.m_Nodes.push_back(node);
    }

    {
        PlantInstanceNode node;
        node.m_RelativePosition = glm::vec3(0.f, _params.m_TrunkBaseHeight, 0.f);
        node.m_ThicknessScale = _params.m_BranchThicknessScaleFactor;
        node.m_Depth = 1;
        result.m_Nodes.push_back(node);
    }

    const u32 branchingDepth = _params.m_MinBranchingDepth
        + (_params.m_MaxBranchingDepth - _params.m_MinBranchingDepth) * unsignedDistribution(gen);

    for (u32 nodeIdx = 1; nodeIdx < result.m_Nodes.size(); ++nodeIdx)
    {
        PlantInstanceNode& currentNode = result.m_Nodes[nodeIdx];

        if (currentNode.m_Depth == branchingDepth)
        {
            continue;
        }

        // TODO fix to use an integer distribution
        const u32 childCount = _params.m_MinBranchingFactor
        + (_params.m_MaxBranchingFactor - _params.m_MinBranchingFactor) * unsignedDistribution(gen);

        currentNode.m_ChildCount = childCount;

        const glm::vec3 branchingBasis = MathsHelpers::GenerateArbitraryNormal(currentNode.m_Normal);

        const f32 branchingOffset = unsignedDistribution(gen) * glm::pi<f32>();
        const f32 branchingSpread = glm::pi<f32>() * 2.f / static_cast<f32>(childCount);
        
        for (u32 childIdx = 0; childIdx < childCount; ++childIdx)
        {
            const u32 newChildNodeIdx = result.m_Nodes.size();

            currentNode.m_Children[childIdx] = newChildNodeIdx;
            
            PlantInstanceNode& newChildNode = result.m_Nodes.emplace_back();
            
            const f32 rotationAngleDegrees = _params.m_MinBranchingAngle + (_params.m_MaxBranchingAngle - _params.m_MinBranchingAngle) * unsignedDistribution(gen);

            const f32 rotationAngle = glm::radians(rotationAngleDegrees);
            
            // Rotate within the specified range from the previous branch direction.
            glm::vec3 newNormal = glm::rotate(currentNode.m_Normal, rotationAngle, branchingBasis);

            // Distribute the branches around the axis of the parent branch.
            newNormal = glm::rotate(newNormal, branchingOffset + (branchingSpread * childIdx), currentNode.m_Normal);
            
            newChildNode.m_Normal = newNormal;
            newChildNode.m_Depth = currentNode.m_Depth + 1;

            const f32 scaleFactor = _params.m_MinBranchScaleFactor + (_params.m_MaxBranchScaleFactor - _params.m_MinBranchScaleFactor) * unsignedDistribution(gen);
            const f32 length = _params.m_TrunkBaseHeight * glm::pow(scaleFactor, newChildNode.m_Depth);

            newChildNode.m_RelativePosition = currentNode.m_RelativePosition + newNormal * length;
            newChildNode.m_ThicknessScale = glm::pow(_params.m_BranchThicknessScaleFactor, newChildNode.m_Depth);

            newChildNode.m_Foliage = (branchingDepth - newChildNode.m_Depth < _params.m_MinFoliageRelativeDepth);
        }

    }

    return result;
}

RawMesh ImportMesh(MeshAssetID _id)
{
    RawMesh result;
    
    std::string modelPath = Globals::FREEPLANET_ASSET_PATH + "models/" + Assets::GetMeshAssetName(_id) + ".obj";
    
    std::optional<MeshImport::ImportedMeshData> mesh = MeshImport::ImportOBJ(modelPath);

    if (!mesh.has_value())
    {
        LogError("Failed to import " + modelPath);
        return result;
    }

    result = MeshImport::ConvertToRawMesh(*mesh);

    return result;
}

RawMesh FloraGeneration::ConvertToRawMesh(const PlantInstance& _plantInstance, const FloraGenerationParams& _params)
{
    RawMesh result;

    const RawMesh branchMesh = ImportMesh(_params.m_BranchMesh);
    const RawMesh narrowBranchMesh = ImportMesh(_params.m_NarrowBranchMesh);
    
    for (const PlantInstanceNode& node : _plantInstance.m_Nodes)
    {
        for (u32 childIdx = 0; childIdx < node.m_ChildCount; ++childIdx)
        {
            const u32 childNodeIdx = node.m_Children[childIdx];
            const glm::vec3 childPosition = _plantInstance.m_Nodes[childNodeIdx].m_RelativePosition;

            const bool isNarrow = _plantInstance.m_Nodes[childNodeIdx].m_ChildCount == 0;

            const glm::vec3 meshOriginOffset = glm::vec3(0.f, 0.f, 0.f);
            const f32 meshXZScale = isNarrow ? 2.f : 1.f;

            // Allow to be slightly oversized in y so that branches protrude from their parent.
            const f32 meshYScale = isNarrow ? 0.5f : 0.6f;
            
            const glm::vec3 normal = glm::normalize(childPosition - node.m_RelativePosition);

            const glm::mat4 offset = glm::translate(meshOriginOffset);
            
            const glm::mat4 rotation = MathsHelpers::GenerateRotationMatrixFromUp(normal);

            const glm::mat4 scale = glm::scale(glm::vec3(
                                                   meshXZScale * node.m_ThicknessScale,
                                                   meshYScale * glm::length(childPosition - node.m_RelativePosition),
                                                   meshXZScale * node.m_ThicknessScale));
            
            RawMesh branch = isNarrow ? narrowBranchMesh : branchMesh;
            branch.Transform(rotation * scale * offset);
            branch.Translate(node.m_RelativePosition);
            result.Append(branch);
        }
    }

    result.m_SupportInstancing = true;
    return result;
}

ParticleSystem FloraGeneration::GenerateFoliage(const PlantInstance& _plantInstance, const FloraGenerationParams& _params)
{
    ParticleSystem result;

    ParticleEmitter& emitter = result.m_Emitters.emplace_back();
    emitter.m_RelativePosition = glm::vec3(0.f);

    emitter.m_Shader = AssetHandle<ShaderProgram>(ShaderAsset_Lit_Inst_AlphaTest_NormalUp);
    emitter.m_Texture = AssetHandle<Texture>(TextureAsset_Billboard_Leaves);

    emitter.m_Animation = ParticleAnimation::PerlinOffset;
    emitter.m_AnimFrequency = 1.5f;
    emitter.m_AnimAmplitude = 0.25f;
    
    for (const PlantInstanceNode& node : _plantInstance.m_Nodes)
    {
        if (node.m_Foliage)
        {
            Particle& particle = emitter.m_Particles.emplace_back();
            particle.m_RelativePosition = node.m_RelativePosition;

            const glm::vec3 centreToParticle = glm::normalize(node.m_RelativePosition - emitter.m_RelativePosition);
            particle.m_Rotation = MathsHelpers::GenerateRotationMatrixFromUp(centreToParticle);
        }
    }

    return result;
}

