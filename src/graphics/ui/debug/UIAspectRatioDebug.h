//
// Created by alastair on 10/05/2020.
//


#pragma once

#include <src/graphics/ui/UIBaseMenu.h>

class UIAspectRatioDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
};



