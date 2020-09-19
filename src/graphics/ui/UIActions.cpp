//
// Created by alastair on 10/04/2020.
//

#include "UIActions.h"

#include <functional>

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
    u64 intSeed = atoi(_seedString.c_str());

    if (intSeed == 0)
    {
        intSeed = std::hash<std::string>{}(_seedString);
    }

    LogMessage("Requesting planet with parsed seed " + std::to_string(static_cast<u32>(intSeed)) + " from string " + _seedString);

    m_UIDisplay->AddEvent(EngineEvent(EngineEvent::Type::EngineLoadPlanetFromSeed, intSeed));
}

void UIActions::SetUIDisplay(UIDisplay* _display)
{
    m_UIDisplay = _display;
}