//
// Created by alastair on 10/04/2020.
//

#include <src/graphics/ui/UIBaseMenu.h>
#include <src/graphics/ui/widgets/UIList.h>

#pragma once

class UINewPlanetMenu : public UIBaseMenu
{
public:
    void Init() override;

    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
    void OnButtonReleased(InputType _type, UIActions& _actions) override;
    void OnTextEntered(std::string _text) override;
    bool ShouldTakeFocus() const override {return true;};

private:
    UIList m_List;
};
