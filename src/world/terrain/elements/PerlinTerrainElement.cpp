//
// Created by alastair on 25/02/18.
//

#include "PerlinTerrainElement.h"

#include <glm/gtc/noise.hpp>
#include <vector>

f32 PerlinTerrainElement::GetDensity(glm::vec3 _position, glm::vec3 _globalOffset) const
{
    const glm::vec3 noisePosition = _position + _globalOffset;

    constexpr f32 scaleFactor = 16.f;
    constexpr f32 amplitude = 8.f;

    const f32 perlinValue = glm::perlin(noisePosition / scaleFactor ) * amplitude;

    // Apply a planar height to the Perlin terrain.
    // Moved this here for now so it's not interfering with the additive element combination in TerrainComponent.
    const f32 resultWithHeight = perlinValue + 10.f - noisePosition.y;

    // Uncomment to test with a spherical terrain.
    // const glm::vec3 planetCore = glm::vec3(0.f, -32.f * 20.f,0.f);
    // const f32 resultWithHeight = perlinValue + (32.f * 20.f + 10.f) - glm::length(noisePosition - planetCore);

    return resultWithHeight;
}
