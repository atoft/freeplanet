//
// Created by alastair on 24/02/2020.
//

#include "UICollisionDebug.h"

#include <src/world/collision/CollisionHandler.h>
#include <src/world/World.h>

void UICollisionDebug::Init(const World* _world)
{
    m_World = _world;
}

void UICollisionDebug::Draw(TimeMS _delta, UIDrawInterface& _display)
{
    m_World->GetCollisionHandler()->DebugDraw(_display);
}
