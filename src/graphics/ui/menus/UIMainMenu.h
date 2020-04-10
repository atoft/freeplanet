//
// Created by alastair on 10/04/2020.
//

#include <src/graphics/ui/UIBaseMenu.h>
#include <src/graphics/ui/widgets/UIList.h>

class UIMainMenu : public UIBaseMenu
{
public:
    void Init() override;

    void Draw(TimeMS _delta, UIDrawInterface& _display, const World* _world) override;
    void OnButtonReleased(InputType _type, UIActions& _actions) override;

private:
    UIList m_List;
};
