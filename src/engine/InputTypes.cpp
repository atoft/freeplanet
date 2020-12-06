//
// Created by alastair on 22/05/2020.
//

#include "InputTypes.h"

#include <src/engine/inspection/BaseInspects.h>
#include <src/engine/inspection/InspectionContext.h>
#include <src/tools/globals.h>

std::string ToString(InputType _inputType)
{
    switch (_inputType)
    {
    case InputType::MoveForward:
        return "MoveForward";
    case InputType::MoveBack:
        return "MoveBack";
    case InputType::TurnLeft:
        return "TurnLeft";
    case InputType::TurnRight:
        return "TurnRight";
    case InputType::LookUp:
        return "LookUp";
    case InputType::LookDown:
        return "LookDown";
    case InputType::StrafeLeft:
        return "StrafeLeft";
    case InputType::StrafeRight:
        return "StrafeRight";
    case InputType::Jump:
        return "Jump";
    case InputType::Sprint:
        return "Sprint";
    case InputType::Interact:
        return "Interact";
    case InputType::InteractAlternate:
        return "InteractAlternate";
    case InputType::InventorySlot1:
        return "InventorySlot1";
    case InputType::InventorySlot2:
        return "InventorySlot2";
    case InputType::InventorySlot3:
        return "InventorySlot3";
    case InputType::InventorySlot4:
        return "InventorySlot4";
    case InputType::InventorySlot5:
        return "InventorySlot5";
    case InputType::LevitateUp:
        return "LevitateUp";
    case InputType::LevitateDown:
        return "LevitateDown";
    case InputType::RollLeft:
        return "RollLeft";
    case InputType::RollRight:
        return "RollRight";
    case InputType::ToggleLog:
        return "ToggleLog";
    case InputType::MenuAccept:
        return "MenuAccept";
    case InputType::MenuBack:
        return "MenuBack";
    case InputType::MenuLeft:
        return "MenuLeft";
    case InputType::MenuRight:
        return "MenuRight";
    case InputType::MenuUp:
        return "MenuUp";
    case InputType::MenuDown:
        return "MenuDown";
    case InputType::EngineQuit:
        return "EngineQuit";
    default:
        break;
    }
    return "";
    static_assert(static_cast<u32>(InputType::Count) == 30);
}

std::string ToString(InputContext _inputContext)
{
    switch (_inputContext)
    {
    case InputContext::None:
        return "None";
    case InputContext::UI:
        return "UI";
    case InputContext::Gameplay:
        return "Gameplay";
    case InputContext::Debug:
        return "Debug";
    default:
        break;
    }

    static_assert(static_cast<u32>(InputContext::Count) == 4);
    return "";
}

std::string ToString(InputButtonInteraction _interaction)
{
    switch (_interaction)
    {
        case InputButtonInteraction::None:
            return "None";
        case InputButtonInteraction::IsDown:
            return "IsDown";
        case InputButtonInteraction::OnPressed:
            return "OnPressed";
        case InputButtonInteraction::OnReleased:
            return "OnReleased";
        default:
            break;
    }

    static_assert(static_cast<u32>(InputButtonInteraction::Count) == 4);
    return "";
}

std::string ToString(KeyboardKey _key)
{
    if (_key >= KeyboardKey::A && _key <= KeyboardKey::Z)
    {
        const u8 character = 65 - static_cast<u8>(sf::Keyboard::A) + static_cast<u8>(_key);
        return std::string(1, character);
    }

    if (_key >= KeyboardKey::Num0 && _key <= KeyboardKey::Num9)
    {
        const u8 character = 48 - static_cast<u8>(sf::Keyboard::Num0) + static_cast<u8>(_key);
        return std::string(1, character);
    }

    switch (_key)
    {
        case KeyboardKey::Space:
            return "Space";
        case KeyboardKey::LShift:
            return "Shift";
        case KeyboardKey::Enter:
            return "Enter";
        case KeyboardKey::Tilde:
            return "Tilde";
        case KeyboardKey::F1:
            return "F1";
        case KeyboardKey::Escape:
            return "Escape";
        case KeyboardKey::Up:
            return "Up";
        case KeyboardKey::Down:
            return "Down";
        case KeyboardKey::Left:
            return "Left";
        case KeyboardKey::Right:
            return "Right";
        default:
            break;
    }
    return "UNHANDLED";
}

void Inspect(std::string _name, InputKeyMapping& _target, InspectionContext& _context)
{
    constexpr u32 version = 0;
    _context.Struct(_name, InspectionType::InputKeyMapping, version);

    Inspect("InputType", _target.m_InputType, _context);
    Inspect("InputContext", _target.m_InputContext, _context);
    Inspect("Key", _target.m_Key, _context);
    Inspect("Interaction", _target.m_Interaction, _context);

    _context.EndStruct();
}
