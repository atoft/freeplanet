//
// Created by alastair on 10/05/2020.
//

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
