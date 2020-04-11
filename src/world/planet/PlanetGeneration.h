//
// Created by alastair on 10/04/2020.
//

#pragma once

#include <src/tools/globals.h>
#include <src/world/planet/Planet.h>

struct WorldPosition;

class PlanetGeneration
{
public:
    static Planet GenerateFromSeed(u32 _seed);

    static glm::vec3 GetUpDirection(const Planet& _planet, const WorldPosition& _position);
private:
    static void GenerateFibonacciSphere(u32 _count, std::vector<glm::vec2>& _outPitchYaws);
};
