//
// Created by alastair on 10/04/2020.
//

#include "UIMainMenu.h"

#include <src/config.h>
#include <src/graphics/ui/UIActions.h>
#include <src/graphics/ui/menus/UIConstants.h>

void UIMainMenu::Init(bool _startFocused)
{
    m_List.AddButton("New Planet", [](UIActions& actions) { actions.GoToNewPlanetMenu(); });
    m_List.AddButton("Quit", [](UIActions& actions) { actions.Quit(); });

    if (_startFocused)
    {
        m_List.Focus();
    }
}

void UIMainMenu::Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world)
{
    const std::string buildInfo = freeplanet_APP_NAME_PRETTY " " + std::to_string(freeplanet_VERSION_MAJOR) + "."
        + std::to_string(freeplanet_VERSION_MINOR);

    _display.FillScreen(UIConstants::BackgroundColor);
    _display.DrawString(UIConstants::ListStartPosition - glm::ivec2(0, 200), "freeplanet", 48.f);
    _display.DrawString(glm::ivec2(UIConstants::ScreenEdgeMargin.x,
                                   UIConstants::UIResolution.y - UIConstants::ScreenEdgeMargin.y - 12.f),
                        buildInfo,
                        12.f,
                        Color(1.f),
                        FontStyle::Monospace,
                        UIAnchorPosition::BottomLeft);

    m_List.Draw(_delta, _display);
}

void UIMainMenu::OnButtonReleased(InputType _type, UIActions& _actions)
{
    m_List.OnButtonReleased(_type, _actions);
}

void UIMainMenu::OnMouseHover(const UIDrawInterface& _display, f32 _x, f32 _y)
{
    m_List.OnMouseHover(_display, _x, _y);
}
