//
// Created by alastair on 03/03/19.
//


#pragma once

#include "UIBaseMenu.h"

class UITerrainDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display) override;
    void Init(const World* _world);

private:
    const World* m_World;
};
