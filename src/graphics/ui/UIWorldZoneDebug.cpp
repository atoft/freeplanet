//
// Created by alastair on 17/12/18.
//

#include "UIWorldZoneDebug.h"

#include <src/world/World.h>

void UIWorldZoneDebug::Init(const World* _world)
{
    m_World = _world;
}

void UIWorldZoneDebug::Draw(TimeMS _delta, UIDrawInterface& _display)
{
    if(m_World == nullptr)
    {
        return;
    }
    m_World->DebugDraw(_display);
}
