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

#include "TestWorld.h"

#include <src/world/World.h>
#include <src/tools/MathsHelpers.h>
#include <src/tools/PropRecipe.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/planet/PlanetGeneration.h>

// TODO Fix this. Make TestWorlds assets that can be loaded from disk.
std::shared_ptr<World> Test::BuildTestWorld(std::string _worldName)
{
    std::shared_ptr<World> world = nullptr;

    if (_worldName == "none")
    {
    }
    else if (_worldName == "terrain")
    {
        const Planet planet = PlanetGeneration::GenerateFromSeed(1);

        world = std::make_shared<World>();
    }
    else if (_worldName == "empty")
    {
        world = std::make_shared<World>();

        PropRecipe planeRecipe;
        planeRecipe.m_TextureID = TextureAsset_Dev_512;
        planeRecipe.m_ShaderID = ShaderAsset_Default;
        planeRecipe.m_MeshID = MeshAsset_UnitCube;
        planeRecipe.m_Name = "Plane";
        planeRecipe.m_Scale = glm::vec3(5.f, 1.f, 5.f);

        world->SpawnPropInWorldZone(WorldPosition(glm::ivec3(0,0,0), glm::vec3(0.f, -10.f, 0.f)), planeRecipe);
    }
    else if (_worldName == "collision")
    {
        world = std::make_shared<World>();

        {
            PropRecipe planeRecipe;
            planeRecipe.m_TextureID = TextureAsset_Dev_512;
            planeRecipe.m_ShaderID = ShaderAsset_Default;
            planeRecipe.m_MeshID = MeshAsset_UnitCube;
            planeRecipe.m_Name = "Plane";
            planeRecipe.m_Scale = glm::vec3(10.f, 1.f, 10.f);

            world->SpawnPropInWorldZone(WorldPosition(glm::ivec3(0, 0, 0), glm::vec3(0.f, -5.f, 0.f)), planeRecipe);;
        }

        {
            PropRecipe rampRecipe;
            rampRecipe.m_TextureID = TextureAsset_Dev_512;
            rampRecipe.m_ShaderID = ShaderAsset_Default;
            rampRecipe.m_MeshID = MeshAsset_UnitCube;
            rampRecipe.m_Name = "Diamond";
            rampRecipe.m_Scale = glm::vec3(10.f, 5.f, 5.f);
            rampRecipe.m_PitchYawRoll = glm::vec3(glm::pi<f32>() / 4.f, 0.f, 0.f);

            world->SpawnPropInWorldZone(WorldPosition(glm::ivec3(0, 0, 0), glm::vec3(0.f, -2.5f, -6.f)), rampRecipe);
        }

        {
            PropRecipe rampRecipe;
            rampRecipe.m_ShaderID = ShaderAsset_Unlit_ShowNormals;
            rampRecipe.m_MeshID = MeshAsset_UnitCube;
            rampRecipe.m_Name = "Ramp";
            rampRecipe.m_Scale = glm::vec3(5.f, 1.f, 10.f);
            rampRecipe.m_PitchYawRoll = glm::vec3(-glm::pi<f32>() / 8.f, 0.f, 0.f);

            world->SpawnPropInWorldZone(WorldPosition(glm::ivec3(0, 0, 0), glm::vec3(0.f, -2.5f, 6.f)), rampRecipe);
        }

        {
            PropRecipe pillarRecipe;
            pillarRecipe.m_TextureID = TextureAsset_Dev_512;
            pillarRecipe.m_ShaderID = ShaderAsset_Default;
            pillarRecipe.m_MeshID = MeshAsset_UnitCube;
            pillarRecipe.m_Name = "Pillar";
            pillarRecipe.m_Scale = glm::vec3(1.f, 5.f, 1.f);
            pillarRecipe.m_PitchYawRoll = glm::vec3(0.f, glm::pi<f32>() / 4.f, 0.f);

            world->SpawnPropInWorldZone(WorldPosition(glm::ivec3(0, 0, 0), glm::vec3(-4.f, -2.f, 2.f)), pillarRecipe);
            world->SpawnPropInWorldZone(WorldPosition(glm::ivec3(0, 0, 0), glm::vec3(4.f, -2.f, 2.f)), pillarRecipe);
        }
    }
    else
    {
        LogError(_worldName + " is not a valid test world.");
        return nullptr;
    }

    return world;
}
