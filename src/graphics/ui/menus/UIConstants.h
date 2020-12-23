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

#include <src/tools/globals.h>
#include <src/graphics/Color.h>

class UIConstants
{
public:
    static constexpr glm::ivec2 UIResolution = glm::ivec2(1920, 1080);

    static constexpr glm::ivec2 ScreenEdgeMargin = glm::ivec2(20, 20);
    
    static constexpr glm::ivec2 ButtonDimensions = glm::ivec2(500, 50);

    static constexpr glm::ivec2 ListStartPosition = glm::ivec2((1920 - ButtonDimensions.x) / 2, 1080 / 2);

    static constexpr u32 ListItemSpacing = ButtonDimensions.y + 30;

    static constexpr Color BackgroundColor = Color(0.282f, 0.615f, 0.878f, 1.f);
};
