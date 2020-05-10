//
// Created by alastair on 10/04/2020.
//

#include "UINewPlanetMenu.h"

#include <src/graphics/ui/menus/UIConstants.h>

void UINewPlanetMenu::Init(bool _startFocused)
{
    m_List.AddTextInput("");
    m_List.AddButton("Create", [this](UIActions& actions){actions.CreatePlanetFromSeed(std::get<UITextInput>(m_List.m_Widgets[0]).GetText());});
    m_List.AddButton("Back", [](UIActions& actions){actions.GoToMainMenu();});

    if (_startFocused)
    {
        m_List.Focus();
    }
}

void UINewPlanetMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    _display.FillScreen(UIConstants::BackgroundColor);
    _display.DrawString(UIConstants::ListStartPosition - glm::ivec2(0,200), "New planet", 48.f);
    m_List.Draw(_delta, _display);

}

void UINewPlanetMenu::OnButtonReleased(InputType _type, UIActions& _actions)
{
    m_List.OnButtonReleased(_type, _actions);
}

void UINewPlanetMenu::OnTextEntered(std::string _text)
{
    // TODO A nice way to access widgets..?
    std::get<UITextInput>(m_List.m_Widgets[0]).OnTextEntered(_text);
}

void UINewPlanetMenu::OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y)
{
    m_List.OnMouseHover(_display, _x, _y);
}
