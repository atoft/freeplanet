//
// Created by alastair on 10/04/2020.
//

#pragma once

#include <string>

class UIDisplay;

class UIActions
{
public:
    void GoToNewPlanetMenu();
    void GoToMainMenu();
    void Quit();

    void CreatePlanetFromSeed(std::string _seedString);

    void SetUIDisplay(UIDisplay* _display);

private:
    UIDisplay* m_UIDisplay = nullptr;
};
