//
// Created by alastair on 17/12/18.
//

#include "UIWorldZoneDebug.h"

#include <src/world/World.h>

void UIWorldZoneDebug::Draw(TimeMS, UIDrawInterface& _display, const World* _world)
{
    if(_world == nullptr)
    {
        return;
    }

    _world->DebugDraw(_display);
}
