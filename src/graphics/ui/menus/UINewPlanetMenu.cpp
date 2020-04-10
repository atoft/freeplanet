//
// Created by alastair on 10/04/2020.
//

#include "UINewPlanetMenu.h"

#include <src/graphics/ui/menus/UIConstants.h>

void UINewPlanetMenu::Init()
{
    m_List.AddTextInput("");
    m_List.AddButton("Create", [this](UIActions& actions){actions.CreatePlanetFromSeed(std::get<UITextInput>(m_List.m_Widgets[0]).GetText());});
    m_List.AddButton("Back", [](UIActions& actions){actions.GoToMainMenu();});
}

void UINewPlanetMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    _display.DrawRectangle(glm::ivec2(0,0), UIConstants::UIResolution, UIConstants::BackgroundColor);
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