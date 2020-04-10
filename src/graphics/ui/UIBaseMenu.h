//
// Created by alastair on 01/10/18.
//

#pragma once

#include <src/graphics/ui/UIDisplay.h>
#include <src/engine/InputTypes.h>
#include <src/tools/globals.h>

class UIDrawInterface;
class UIActions;
class World;

class UIBaseMenu
{
public:
    virtual void Init() {};
    virtual void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) = 0;
    virtual void OnTextEntered(std::string _text) {};
    virtual void OnButtonReleased(InputType _type, UIActions& _actions) {};
    virtual bool ShouldTakeFocus() const { return false; };
    virtual ~UIBaseMenu() = default;
};

