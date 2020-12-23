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

#include "src/assets/MeshAssets.h"
#include "src/assets/ShaderAssets.h"
#include "src/assets/TextureAssets.h"
#include <vector>
#include <src/tools/globals.h>
#include <src/graphics/RawMesh.h>

struct FloraGenerationParams
{
    f32 m_TrunkBaseHeight = 3.f;
    f32 m_MinBranchScaleFactor = 0.7f;
    f32 m_MaxBranchScaleFactor = 0.9f;
    f32 m_BranchThicknessScaleFactor = 0.6f;
    
    u32 m_MinBranchingFactor = 2;
    u32 m_MaxBranchingFactor = 4;

    u32 m_MinBranchingDepth = 4;
    u32 m_MaxBranchingDepth = 7;

    u32 m_MinFoliageRelativeDepth = 2;
    
    f32 m_MinBranchingAngle = 20.f;
    f32 m_MaxBranchingAngle = 60.f;

    // TODO probably want pools of meshes we can pick from, and optionally different meshes as the branch thickness changes.
    MeshAssetID m_BranchMesh = MeshAsset_Branch_01;
    MeshAssetID m_NarrowBranchMesh = MeshAsset_Branch_Narrow_01;

    // TODO as the final mesh will be made out of multiple meshes with different texturing, need to send a texture index for each vert.
    TextureAssetID m_BranchTexture = TextureAsset_Branch_01_01;
    ShaderAssetID m_BranchShader = ShaderAsset_Lit_Inst_Textured;
};

constexpr u32 MaxPossibleFloraBranchingFactor = 6;

struct PlantInstanceNode
{
    glm::vec3 m_RelativePosition = glm::vec3(0.f);
    f32 m_ThicknessScale = 1.f;
    
    u32 m_Children[MaxPossibleFloraBranchingFactor];
    u32 m_ChildCount = 0;

    u32 m_Depth = 0;
    glm::vec3 m_Normal = glm::vec3(0.f, 1.f, 0.f);

    bool m_Foliage = false;
};

struct PlantInstance
{
    std::vector<PlantInstanceNode> m_Nodes;
};

class FloraGeneration
{
public:
    static PlantInstance GeneratePlant(const FloraGenerationParams& _params, u32 _seed);
    static RawMesh ConvertToRawMesh(const PlantInstance& _plantInstance, const FloraGenerationParams& _params);
    static struct ParticleSystem GenerateFoliage(const PlantInstance& _plantInstance, const FloraGenerationParams& _params);
};

