//
// Created by alastair on 10/04/2020.
//

#pragma once

#include <src/tools/globals.h>

class UIConstants
{
public:
    static constexpr glm::ivec2 ButtonDimensions = glm::ivec2(500, 50);

    static constexpr glm::ivec2 ListStartPosition = glm::ivec2((1920 - ButtonDimensions.x) / 2, 1080 / 2);

    static constexpr u32 ListItemSpacing = ButtonDimensions.y + 30;
};