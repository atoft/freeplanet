//
// Created by alastair on 24/02/2020.
//

#pragma once

#include <src/graphics/ui/UIBaseMenu.h>

class UICollisionDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display) override;
    void Init(const World* _world);

private:
    const World* m_World = nullptr;
};
