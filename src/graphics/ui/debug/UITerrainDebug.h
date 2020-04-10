//
// Created by alastair on 03/03/19.
//


#pragma once

#include "src/graphics/ui/UIBaseMenu.h"

class UITerrainDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
};
