//
// Created by alastair on 10/04/2020.
//

#include <src/graphics/ui/UIActions.h>
#include "UIMainMenu.h"
#include "UIConstants.h"


void UIMainMenu::Init(bool _startFocused)
{
    m_List.AddButton("New Planet", [](UIActions& actions){ actions.GoToNewPlanetMenu();});
    m_List.AddButton("Quit", [](UIActions& actions){actions.Quit();});

    if (_startFocused)
    {
        m_List.Focus();
    }
}

void UIMainMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    _display.FillScreen(UIConstants::BackgroundColor);
    _display.DrawString(UIConstants::ListStartPosition - glm::ivec2(0,200), "freeplanet", 48.f);

    m_List.Draw(_delta, _display);
}

void UIMainMenu::OnButtonReleased(InputType _type, UIActions& _actions)
{
    m_List.OnButtonReleased(_type, _actions);
}

void UIMainMenu::OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y)
{
    m_List.OnMouseHover(_display, _x, _y);
}
