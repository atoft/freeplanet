//
// Created by alastair on 16/10/19.
//


#pragma once


#include "src/graphics/ui/UIBaseMenu.h"

class Profiler;

class UIProfilingDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
    void Init(const Profiler* _profiler);

private:
    const Profiler* m_Profiler = nullptr;
};
