//
// Created by alastair on 01/10/18.
//

#pragma once

#include <src/tools/globals.h>

class UIDrawInterface;

class UITextInput
{
protected:
    std::string m_InputBuffer;
    u32 m_CaretPosition = 0;

public:
    void OnTextEntered(std::string _text);
    const std::string& GetText() { return m_InputBuffer;};
    void SetText(const std::string& _text);
    void Clear();
    void MoveCaretLeft();
    void MoveCaretRight();

    void Draw(TimeMS _delta, UIDrawInterface& _display, glm::vec2 _position, glm::vec2 _dimensions, float _fontSize);
};
