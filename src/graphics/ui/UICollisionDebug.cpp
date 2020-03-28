//
// Created by alastair on 24/02/2020.
//

#include "UICollisionDebug.h"

#include <src/world/collision/CollisionHandler.h>
#include <src/world/World.h>

void UICollisionDebug::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    if (_world != nullptr)
    {
        _world->GetCollisionHandler()->DebugDraw(_display);
    }
}
