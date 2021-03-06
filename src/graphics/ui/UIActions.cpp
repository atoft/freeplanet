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

#include "UIActions.h"

#include <functional>

#include <src/graphics/ui/UIDisplay.h>

#include <src/graphics/ui/menus/UILoadPlanetMenu.h>
#include <src/graphics/ui/menus/UIMainMenu.h>
#include <src/graphics/ui/menus/UINewPlanetMenu.h>

void UIActions::GoToNewPlanetMenu()
{
    m_UIDisplay->RequestMenu<UINewPlanetMenu>();
}

void UIActions::GoToLoadPlanetMenu()
{
    m_UIDisplay->RequestMenu<UILoadPlanetMenu>();
}

void UIActions::GoToMainMenu()
{
    m_UIDisplay->RequestMenu<UIMainMenu>();
}

void UIActions::Quit()
{
    m_UIDisplay->RequestQuit();
}

void UIActions::NewPlanetFromSeed(const std::string& _nameString, const std::string& _seedString)
{
    u64 intSeed = atoi(_seedString.c_str());

    if (intSeed == 0)
    {
        intSeed = std::hash<std::string>{}(_seedString);
    }

    LogMessage("Requesting planet with parsed seed " + std::to_string(static_cast<u32>(intSeed)) + " from string " + _seedString);

    EnginePlanetRequest request;
    request.m_Name = _nameString;
    request.m_Seed = intSeed;

    m_UIDisplay->AddEvent(EngineEvent(EngineEvent::Type::EngineNewPlanet, request));
}

void UIActions::LoadPlanetFromName(const std::string& _name)
{
    m_UIDisplay->AddEvent(EngineEvent(EngineEvent::Type::EngineLoadPlanet, _name));
}

void UIActions::SetUIDisplay(UIDisplay* _display)
{
    m_UIDisplay = _display;
}
