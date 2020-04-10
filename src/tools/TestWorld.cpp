//
// Created by alastair on 27/11/17.
//

#include "TestWorld.h"

#include <random>

#include <src/world/World.h>
#include <src/tools/MathsHelpers.h>
#include <src/tools/PropRecipe.h>
#include <src/world/terrain/TerrainConstants.h>
#include <src/world/planet/TerrainGeneration.h>

std::shared_ptr<World> Test::BuildTestWorld(std::string _worldName)
{
    std::shared_ptr<World> world = nullptr;

    if (_worldName == "none")
    {
    }
    else if (_worldName == "terrain")
    {
        Planet planet;
        planet.m_TerrainSeed = 1;

        constexpr u32 biomeCount = 100;

        std::mt19937 gen(planet.m_TerrainSeed);
        std::uniform_real_distribution<> unsignedDistribution(0.f, 1.f);
        std::uniform_real_distribution<> signedDistribution(-1.f, 1.f);

        std::vector<glm::vec2> pitchYaws;
        TerrainGeneration::GenerateFibonacciSphere(biomeCount, pitchYaws);

        for (u32 biomeIdx = 0; biomeIdx < biomeCount; ++biomeIdx)
        {
            Planet::Biome greenBiome;
            greenBiome.m_GroundColor = Color(unsignedDistribution(gen), unsignedDistribution(gen), unsignedDistribution(gen), 1);
            greenBiome.m_PitchRadians = pitchYaws[biomeIdx].x;
            greenBiome.m_YawRadians = pitchYaws[biomeIdx].y;
            greenBiome.m_BiomeDirection = MathsHelpers::GenerateNormalFromPitchYaw(greenBiome.m_PitchRadians,
                                                                                   greenBiome.m_YawRadians);

            for (f32& weight : greenBiome.m_Inputs.m_OctaveWeights)
            {
                weight = 1.375f + signedDistribution(gen) * 0.625f;
            }

            planet.m_Biomes.push_back(greenBiome);
        }

        world = std::make_shared<World>("Terrain", planet);

        // HACK copy-paste from World.cpp
        // In the real flow we need to be able to request the world to add a player, it will handle loading the relevant
        // zones, and decide when everything's ready to spawn the player.
        const glm::ivec3 spawnZoneCoordinates = glm::ivec3(0, planet.m_Radius / TerrainConstants::WORLD_ZONE_SIZE, 0);

        world->SpawnPlayerInWorldZone(spawnZoneCoordinates);

        PropRecipe boxRecipe;
        boxRecipe.m_TextureID = TextureAsset_Dev_512;
        boxRecipe.m_ShaderID = ShaderAsset_Default;
        boxRecipe.m_MeshID = MeshAsset_UnitCube;
        boxRecipe.m_Name = "Box";

        world->SpawnPropInWorldZone(WorldPosition(spawnZoneCoordinates, glm::vec3(10.f,2.25f,-5.25f)), boxRecipe);
        world->SpawnPropInWorldZone(WorldPosition(spawnZoneCoordinates, glm::vec3(0.f, 1.f, -5.f)), boxRecipe);
    }
    else if (_worldName == "empty")
    {
        world = std::make_shared<World>("Empty", std::nullopt);
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
        world = std::make_shared<World>("Collision", std::nullopt);
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
