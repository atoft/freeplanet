//
// Created by alastair on 01/10/18.
//

#include "UITextInput.h"

#include <string>

#include <src/graphics/ui/UIDrawInterface.h>

void UITextInput::OnTextEntered(std::string _text)
{
    // Assumes single length characters!
    // ASCII is acceptable for console input but this class will need to be updated
    // to be useful in real UI
    if(_text == "\b")
    {
        if(m_CaretPosition != 0)
        {
            m_InputBuffer.erase(m_CaretPosition - 1, 1);
            --m_CaretPosition;
        }
    }
    else if(_text == "\u007f")
    {
        if(m_CaretPosition < m_InputBuffer.size())
        {
            m_InputBuffer.erase(m_CaretPosition, 1);
        }
    }
    else if(_text == "\u0060")
    {
        // Ignore tilde
    }
    else if(_text == "\u000d")
    {
        // Ignore return
    }
    else if (_text == "\u000a")
    {
        // Ignore return
    }
    else
    {
        m_InputBuffer.insert(m_CaretPosition, _text);
        ++m_CaretPosition;
    }
}

void UITextInput::SetText(const std::string& _text)
{
    m_InputBuffer = _text;
    m_CaretPosition = m_InputBuffer.size();
}

void UITextInput::Clear()
{
    m_InputBuffer.clear();
    m_CaretPosition = 0;
}

void UITextInput::MoveCaretLeft()
{
    m_CaretPosition = (m_CaretPosition == 0) ? m_CaretPosition : m_CaretPosition - 1;
}

void UITextInput::MoveCaretRight()
{
    m_CaretPosition = (m_CaretPosition == m_InputBuffer.size()) ? m_CaretPosition : m_CaretPosition + 1;
}

void UITextInput::Draw(TimeMS _delta, UIDrawInterface& _display, bool _isFocused)
{
    _display.DrawRectangle(m_Position, m_Dimensions, Color(0.1f, 0.1f, 0.1f, 1.f));
    _display.DrawString(m_Position, m_InputBuffer, m_FontSize, Color(0,1.f,0,1.f), FontStyle::Monospace, UIAnchorPosition::Centered, false, m_CaretPosition);
}
