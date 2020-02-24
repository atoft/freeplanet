//
// Created by alastair on 27/11/17.
//

#include "TestWorld.h"

#include <src/world/World.h>
#include <src/tools/PropRecipe.h>

std::shared_ptr<World> Test::BuildTestWorld(std::string _worldName)
{
    std::shared_ptr<World> world = nullptr;

    if (_worldName == "none")
    {
    }
    else if (_worldName == "terrain")
    {
        constexpr u32 TEST_WORLD_SEED = 0;

        world = std::make_shared<World>("Terrain", TEST_WORLD_SEED, true);
        world->SpawnPlayerInWorldZone(glm::ivec3(0,0,0));

        PropRecipe boxRecipe;
        boxRecipe.m_TextureID = TextureAsset_Dev_512;
        boxRecipe.m_ShaderID = ShaderAsset_Default;
        boxRecipe.m_MeshID = MeshAsset_UnitCube;
        boxRecipe.m_Name = "Box";

        world->SpawnPropInWorldZone(WorldPosition(glm::ivec3(0,0,0), glm::vec3(10.f,1.25f,-5.25f)), boxRecipe);
        world->SpawnPropInWorldZone(WorldPosition(glm::ivec3(0,0,0), glm::vec3(0.f, 0.f, -5.f)), boxRecipe);
    }
    else if (_worldName == "empty")
    {
        constexpr u32 TEST_WORLD_SEED = 0;

        world = std::make_shared<World>("Empty", TEST_WORLD_SEED, false);
        world->SpawnPlayerInWorldZone(glm::ivec3(0,0,0));

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
        constexpr u32 TEST_WORLD_SEED = 0;

        world = std::make_shared<World>("Collision", TEST_WORLD_SEED, false);
        world->SpawnPlayerInWorldZone(glm::ivec3(0,0,0));

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
