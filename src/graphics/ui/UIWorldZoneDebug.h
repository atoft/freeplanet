//
// Created by alastair on 17/12/18.
//

#pragma once

#include "UIBaseMenu.h"

class UIWorldZoneDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
};
