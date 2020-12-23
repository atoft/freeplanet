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

#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <src/tools/globals.h>

// Project-specific version of sf::Keyboard::Key.
// Supports Inspection.
enum class KeyboardKey
{
    Unknown = -1, ///< Unhandled key
    A = 0,        ///< The A key
    B,            ///< The B key
    C,            ///< The C key
    D,            ///< The D key
    E,            ///< The E key
    F,            ///< The F key
    G,            ///< The G key
    H,            ///< The H key
    I,            ///< The I key
    J,            ///< The J key
    K,            ///< The K key
    L,            ///< The L key
    M,            ///< The M key
    N,            ///< The N key
    O,            ///< The O key
    P,            ///< The P key
    Q,            ///< The Q key
    R,            ///< The R key
    S,            ///< The S key
    T,            ///< The T key
    U,            ///< The U key
    V,            ///< The V key
    W,            ///< The W key
    X,            ///< The X key
    Y,            ///< The Y key
    Z,            ///< The Z key
    Num0,         ///< The 0 key
    Num1,         ///< The 1 key
    Num2,         ///< The 2 key
    Num3,         ///< The 3 key
    Num4,         ///< The 4 key
    Num5,         ///< The 5 key
    Num6,         ///< The 6 key
    Num7,         ///< The 7 key
    Num8,         ///< The 8 key
    Num9,         ///< The 9 key
    Escape,       ///< The Escape key
    LControl,     ///< The left Control key
    LShift,       ///< The left Shift key
    LAlt,         ///< The left Alt key
    LSystem,      ///< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
    RControl,     ///< The right Control key
    RShift,       ///< The right Shift key
    RAlt,         ///< The right Alt key
    RSystem,      ///< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
    Menu,         ///< The Menu key
    LBracket,     ///< The [ key
    RBracket,     ///< The ] key
    Semicolon,    ///< The ; key
    Comma,        ///< The , key
    Period,       ///< The . key
    Quote,        ///< The ' key
    Slash,        ///< The / key
    Backslash,    ///< The \ key
    Tilde,        ///< The ~ key
    Equal,        ///< The = key
    Hyphen,       ///< The - key (hyphen)
    Space,        ///< The Space key
    Enter,        ///< The Enter/Return keys
    Backspace,    ///< The Backspace key
    Tab,          ///< The Tabulation key
    PageUp,       ///< The Page up key
    PageDown,     ///< The Page down key
    End,          ///< The End key
    Home,         ///< The Home key
    Insert,       ///< The Insert key
    Delete,       ///< The Delete key
    Add,          ///< The + key
    Subtract,     ///< The - key (minus, usually from numpad)
    Multiply,     ///< The * key
    Divide,       ///< The / key
    Left,         ///< Left arrow
    Right,        ///< Right arrow
    Up,           ///< Up arrow
    Down,         ///< Down arrow
    Numpad0,      ///< The numpad 0 key
    Numpad1,      ///< The numpad 1 key
    Numpad2,      ///< The numpad 2 key
    Numpad3,      ///< The numpad 3 key
    Numpad4,      ///< The numpad 4 key
    Numpad5,      ///< The numpad 5 key
    Numpad6,      ///< The numpad 6 key
    Numpad7,      ///< The numpad 7 key
    Numpad8,      ///< The numpad 8 key
    Numpad9,      ///< The numpad 9 key
    F1,           ///< The F1 key
    F2,           ///< The F2 key
    F3,           ///< The F3 key
    F4,           ///< The F4 key
    F5,           ///< The F5 key
    F6,           ///< The F6 key
    F7,           ///< The F7 key
    F8,           ///< The F8 key
    F9,           ///< The F9 key
    F10,          ///< The F10 key
    F11,          ///< The F11 key
    F12,          ///< The F12 key
    F13,          ///< The F13 key
    F14,          ///< The F14 key
    F15,          ///< The F15 key
    Pause,        ///< The Pause key

    Count,     ///< Keep last -- the total number of keyboard keys
};

static_assert(static_cast<s32>(sf::Keyboard::Key::Unknown) == static_cast<s32>(KeyboardKey::Unknown));
static_assert(static_cast<s32>(sf::Keyboard::Key::KeyCount) == static_cast<s32>(KeyboardKey::Count));
