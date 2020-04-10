//
// Created by alastair on 10/04/2020.
//

#pragma once

#include <variant>

#include <src/graphics/ui/widgets/UIButton.h>
#include <src/engine/InputTypes.h>
#include <src/graphics/ui/UITextInput.h>

using UIWidgetVariant = std::variant<UIButton, UITextInput>;

class UIList
{
public:
    void AddButton(std::string _label, std::function<void(UIActions&)> _onPressed);
    void AddTextInput(std::string _label);

    void OnButtonReleased(InputType _type, UIActions& _actions);
    void Draw(TimeMS _delta, UIDrawInterface& _display);

public:
    std::vector<UIWidgetVariant> m_Widgets;

private:
    u32 m_ActiveIndex = 0;
};
