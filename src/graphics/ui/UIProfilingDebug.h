//
// Created by alastair on 16/10/19.
//


#pragma once


#include "UIBaseMenu.h"

class Profiler;

class UIProfilingDebug : public UIBaseMenu
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display) override;
    void Init(const Profiler* _profiler);

private:
    const Profiler* m_Profiler = nullptr;
};
