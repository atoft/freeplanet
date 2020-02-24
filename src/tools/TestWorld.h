//
// Created by alastair on 27/11/17.
//

#pragma once

#include <memory>
#include <string>

class World;

namespace Test
{
    std::shared_ptr<World> BuildTestWorld(std::string _worldName);

}
