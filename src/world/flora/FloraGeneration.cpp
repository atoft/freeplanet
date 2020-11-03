/* 
 * Copyright 2020 Alastair Toft
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "FloraGeneration.h"

#include <glm/fwd.hpp>
#include <glm/gtx/rotate_vector.hpp>
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
            
        }

    }

    return result;
}

