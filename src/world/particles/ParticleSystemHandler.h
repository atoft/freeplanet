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

#include <random>
#include <src/tools/globals.h>

class World;
struct ParticleEmitter;

class ParticleSystemHandler
{
public:
    explicit ParticleSystemHandler(World* _world);

    void Update(TimeMS _delta);

private:
    void UpdateEmitter(ParticleEmitter& _emitter, TimeMS _delta);
    
private:
    World* m_World = nullptr;
    std::mt19937 m_RandomGenerator;
};
