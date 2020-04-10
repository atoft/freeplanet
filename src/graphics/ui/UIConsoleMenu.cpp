//
// Created by alastair on 01/10/18.
//

#include "UIConsoleMenu.h"

#include <src/tools/MessageLog.h>
#include <src/graphics/ui/UIDrawInterface.h>
#include <src/engine/events/EngineEvent.h>
#include <src/engine/Engine.h>

UIConsoleMenu::UIConsoleMenu()
{
    m_HistoryBuffer.reserve(CONSOLE_HISTORY_SIZE);
    m_HistoryBuffer.push_back("");

    m_Textbox.m_Position = glm::vec2(0, 1080 - 40);
    m_Textbox.m_Dimensions = glm::vec2(1920,32);
    m_Textbox.m_FontSize = 32;
}

long lineCount(std::string inString)
{
    return 1 + std::count(inString.begin(), inString.end(), '\n');
}

void UIConsoleMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World*)
{
    _display.DrawRectangle(glm::vec2(0), glm::ivec2(1920,1080), Color(0,0,0,0.5f));

    const std::vector<std::pair<LogType ,std::string>> log = MessageLog::GetInstance().GetMessageBuffer();
    float currentY = _display.GetDisplayResolution().y - 32;

    constexpr u32 MARGIN_Y = 5;
    constexpr u32 CONSOLE_LOG_SIZE = 16;

    currentY-= MARGIN_Y + CONSOLE_LOG_SIZE;

    m_Textbox.Draw(_delta, _display, true);

    for(s32 logIdx = log.size() -1; logIdx >=0; --logIdx)
    {
        std::string logDisplay = log[logIdx].second;

        currentY -= (CONSOLE_LOG_SIZE + 4) * lineCount(logDisplay);

        if(currentY < MARGIN_Y)
        {
            break;
        }

        Color fontColor = Color(1.f,1.f,1.f,1.f);

        switch (log[logIdx].first)
        {
            case LogType::freeplanetMESSAGE:
            {
                break;
            }
            case LogType::freeplanetWARNING:
            {
                fontColor = Color(1.f, 0.65f, 0, 1.f);
                break;
            }
            case LogType::freeplanetERROR:
            {
                fontColor = Color(1,0,0,1);
                break;
            }

        }

        _display.DrawString(glm::vec2(10.f, currentY), logDisplay, CONSOLE_LOG_SIZE, fontColor, FontStyle::Sans, true);

    }
}

void UIConsoleMenu::OnTextEntered(std::string _text)
{
    m_Textbox.OnTextEntered(_text);
}

void UIConsoleMenu::OnButtonReleased(InputType _type, UIActions& _actions)
{
    switch(_type)
    {
        case InputType::MenuAccept:
        {
            if(!m_Textbox.GetText().empty())
            {
                if(m_HistoryBuffer.size() == CONSOLE_HISTORY_SIZE)
                {
                    m_HistoryBuffer.erase(m_HistoryBuffer.begin());
                }

                m_HistoryBuffer.push_back(m_Textbox.GetText());
                m_HistoryIdx = m_HistoryBuffer.size();

                auto parsedEvent = EngineEvent::ParseEvent(m_Textbox.GetText());
                if(parsedEvent)
                {
                    Engine::GetInstance().AddEvent(*parsedEvent);
                }

                m_Textbox.Clear();
            }
            break;
        }
        case InputType::MenuLeft:
        {
            m_Textbox.MoveCaretLeft();
            break;
        }
        case InputType::MenuRight:
        {
            m_Textbox.MoveCaretRight();
            break;
        }
        case InputType::MenuUp:
        {
            m_HistoryIdx = glm::max(static_cast<u32>(1), m_HistoryIdx) - 1;
            m_Textbox.SetText(m_HistoryBuffer[m_HistoryIdx]);

            break;

        }
        case InputType::MenuDown:
        {
            m_HistoryIdx = glm::min(static_cast<u32>(m_HistoryBuffer.size()), m_HistoryIdx + 1);

            if(m_HistoryIdx < m_HistoryBuffer.size())
            {
                m_Textbox.SetText(m_HistoryBuffer[m_HistoryIdx]);
            }
            else
            {
                m_Textbox.SetText("");
            }

            break;

        }
        default:
            break;
    }
}
