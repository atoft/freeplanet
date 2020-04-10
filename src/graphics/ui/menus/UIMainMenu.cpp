//
// Created by alastair on 10/04/2020.
//

#include <src/graphics/ui/UIActions.h>
#include "UIMainMenu.h"
#include "UIConstants.h"


void UIMainMenu::Init()
{
    m_List.AddButton("New Planet", [](UIActions& actions){ actions.GoToNewPlanetMenu();});
    m_List.AddButton("Quit", [](UIActions& actions){actions.Quit();});
}

void UIMainMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    _display.DrawRectangle(glm::ivec2(0,0), UIConstants::UIResolution, UIConstants::BackgroundColor);
    _display.DrawString(UIConstants::ListStartPosition - glm::ivec2(0,200), "freeplanet", 48.f);

    m_List.Draw(_delta, _display);
}

void UIMainMenu::OnButtonReleased(InputType _type, UIActions& _actions)
{
    m_List.OnButtonReleased(_type, _actions);
}
