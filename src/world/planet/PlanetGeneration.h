//
// Created by alastair on 10/04/2020.
//

#pragma once

#include <src/tools/globals.h>
#include <src/world/planet/Planet.h>

class PlanetGeneration
{
public:
    static Planet GenerateFromSeed(u32 _seed);

private:
    static void GenerateFibonacciSphere(u32 _count, std::vector<glm::vec2>& _outPitchYaws);
};
