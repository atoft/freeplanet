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

#pragma once

#include <vector>
#include <src/tools/globals.h>

struct FloraGenerationParams
{
    f32 m_TrunkBaseHeight = 3.f;
    f32 m_MinBranchScaleFactor = 0.7f;
    f32 m_MaxBranchScaleFactor = 0.9f;
    
    u32 m_MinBranchingFactor = 2;
    u32 m_MaxBranchingFactor = 4;

    u32 m_MinBranchingDepth = 3;
    u32 m_MaxBranchingDepth = 4;

    f32 m_MinBranchingAngle = 20.f;
    f32 m_MaxBranchingAngle = 60.f;
};

constexpr u32 MaxPossibleFloraBranchingFactor = 6;

struct PlantInstanceNode
{
    glm::vec3 m_RelativePosition = glm::vec3(0.f);

    u32 m_Children[MaxPossibleFloraBranchingFactor];
    u32 m_ChildCount = 0;

    u32 m_Depth = 0;
    glm::vec3 m_Normal = glm::vec3(0.f, 1.f, 0.f);
};

struct PlantInstance
{
    std::vector<PlantInstanceNode> m_Nodes;
};

class FloraGeneration
{
public:
    static PlantInstance GeneratePlant(const FloraGenerationParams& _params, u32 _seed);
};

