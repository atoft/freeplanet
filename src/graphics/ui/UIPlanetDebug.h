//
// Created by alastair on 04/04/2020.
//

#include "UIBaseMenu.h"

class UIPlanetDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
};
