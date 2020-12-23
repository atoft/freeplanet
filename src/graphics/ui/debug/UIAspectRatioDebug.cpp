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

#include "UIAspectRatioDebug.h"

#include <src/graphics/ui/menus/UIConstants.h>

void UIAspectRatioDebug::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    _display.FillScreen(Color(0.2f, 0.2f, 0.f, 1.f));

    _display.DrawRectangle(glm::vec2(0.f,0.f), UIConstants::UIResolution, Color(0.f,1.f,0.f,1.f));
    _display.DrawString(glm::vec2(0.f,0.f), "Full widescreen rectangle", 32.f);

    _display.DrawRectangle(glm::vec2(50.f,50.f), glm::vec2(200.f, 200.f), Color(1.f,0.f,0.f,1.f));
    _display.DrawString(glm::vec2(50.f,50.f), "Square", 32.f);

    _display.DrawRectangle(UIConstants::UIResolution / 2 - glm::ivec2(50,50), glm::vec2(100.f, 100.f), Color(0.f,1.f,1.f,1.f));
    _display.DrawString(UIConstants::UIResolution / 2 - glm::ivec2(50,50), "Centre of screen", 32.f);

    _display.DrawRectangle(glm::vec2(0, UIConstants::UIResolution.y - 32.f), glm::vec2(500.f, 32.f), Color(0.f,0.5f,0.5f,1.f), UIAnchorPosition::BottomLeft);
    _display.DrawString(glm::vec2(0, UIConstants::UIResolution.y - 32.f), "Anchored to botton left", 32.f, Color(1.f), FontStyle::Sans, UIAnchorPosition::BottomLeft);

    _display.DrawRectangle(glm::vec2(0), glm::vec2(500.f, 32.f), Color(0.f,0.5f,0.5f,0.5f), UIAnchorPosition::TopLeft);
    _display.DrawString(glm::vec2(0), "Anchored to top left", 32.f, Color(1.f), FontStyle::Sans, UIAnchorPosition::TopLeft);
}
