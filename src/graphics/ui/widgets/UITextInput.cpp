/*
 * Copyright 2017-2020 Alastair Toft
 *
 * This file is part of freeplanet.
 *
 * freeplanet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeplanet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeplanet. If not, see <http://www.gnu.org/licenses/>.
 */

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

void UITextInput::Draw(TimeMS _delta, UIDrawInterface& _display, bool _isFocused, bool _isActive)
{
    _display.DrawString(m_Position, m_Label, 24.f, Color(1.f), FontStyle::Sans, UIAnchorPosition::Centered, false);

    const f32 labelSpacing = m_Label.empty() ? 0.f : 0.3f;
    const glm::ivec2 boxPosition = m_Position + glm::ivec2(m_Dimensions.x * labelSpacing, 0.f);
    const glm::ivec2 boxDimensions = glm::ivec2(m_Dimensions.x * (1.f - labelSpacing), m_Dimensions.y);

    _display.DrawRectangle(boxPosition, boxDimensions, _isActive ? Color(0.1f, 0.1f, 0.1f, 1.f) : Color(0.3f, 0.3f, 0.3f, 1.f));

    _display.DrawString(
        boxPosition,
        m_InputBuffer,
        m_FontSize,
        Color(0,1.f,0,1.f),
        FontStyle::Monospace,
        UIAnchorPosition::Centered,
        false,
        _isActive ? static_cast<s32>(m_CaretPosition) : -1);
}

bool UITextInput::IsHovered(const UIDrawInterface& _display, f32 _x, f32 _y) const
{
    return _display.IsInAABB(glm::uvec2(_x, _y), m_Position, m_Dimensions, UIAnchorPosition::Centered);
}
