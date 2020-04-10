//
// Created by alastair on 10/04/2020.
//

#include "UIActions.h"

#include <src/graphics/ui/UIDisplay.h>

#include <src/graphics/ui/menus/UINewPlanetMenu.h>
#include <src/graphics/ui/menus/UIMainMenu.h>


void UIActions::GoToNewPlanetMenu()
{
    m_UIDisplay->RequestMenu<UINewPlanetMenu>();
}

void UIActions::GoToMainMenu()
{
    m_UIDisplay->RequestMenu<UIMainMenu>();
}

void UIActions::Quit()
{
    m_UIDisplay->RequestQuit();
}

void UIActions::CreatePlanetFromSeed(std::string _seedString)
{
    LogMessage(_seedString);

    const u64 intSeed = atoi(_seedString.c_str());

    m_UIDisplay->AddEvent(EngineEvent(EngineEvent::Type::EngineLoadPlanetFromSeed, intSeed));
    m_UIDisplay->CloseGameMenu();
}

void UIActions::SetUIDisplay(UIDisplay* _display)
{
    m_UIDisplay = _display;
}