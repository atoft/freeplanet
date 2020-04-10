//
// Created by alastair on 01/10/18.
//

#pragma once

#include <src/tools/globals.h>

class UIDrawInterface;
class UIActions;

class UITextInput
{
public:
    void OnTextEntered(std::string _text);
    const std::string& GetText() { return m_InputBuffer;};
    void SetText(const std::string& _text);
    void Clear();
    void MoveCaretLeft();
    void MoveCaretRight();

    void Draw(TimeMS _delta, UIDrawInterface& _display, bool _isFocused);
    void OnPressed(UIActions& _actions) {};

public:
    glm::ivec2 m_Position = glm::ivec2();
    glm::ivec2 m_Dimensions = glm::ivec2();
    f32 m_FontSize = 32.f;

private:
    std::string m_InputBuffer;
    u32 m_CaretPosition = 0;
};
