//
// Created by alastair on 01/10/18.
//

#pragma once

#include <src/graphics/ui/UIBaseMenu.h>
#include <src/graphics/ui/UITextInput.h>

class UIConsoleMenu : public UIBaseMenu
{
public:
    UIConsoleMenu();
    void Draw(TimeMS _delta, UIDrawInterface& _display) override;
    void OnTextEntered(std::string _text) override;
    void OnButtonReleased(InputType _type) override;

private:
    UITextInput m_Textbox;
    std::vector<std::string> m_HistoryBuffer;
    u32 m_HistoryIdx = 0;

    static constexpr u32 CONSOLE_HISTORY_SIZE = 32;
};
