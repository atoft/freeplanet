//
// Created by alastair on 17/12/18.
//

#pragma once

#include "src/graphics/ui/UIBaseMenu.h"

class UIComponentDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
};
