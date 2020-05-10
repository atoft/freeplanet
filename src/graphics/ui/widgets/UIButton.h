//
// Created by alastair on 10/04/2020.
//

#pragma once

#include <functional>

#include <src/graphics/ui/UIDrawInterface.h>

class UIActions;

class UIButton
{
public:
    void Draw(TimeMS _delta, UIDrawInterface& _display, bool _isFocused);
    void OnPressed(UIActions& _actions);
    bool IsHovered(const UIDrawInterface& _display, f32 _x, f32 _y) const;

public:
    std::string m_Label;
    glm::ivec2 m_Position = glm::ivec2();
    glm::ivec2 m_Dimensions = glm::ivec2();

    std::function<void(UIActions&)> m_OnPressed;
};
