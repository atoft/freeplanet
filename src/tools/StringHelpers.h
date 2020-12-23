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

#include <string>
#include <vector>

class StringHelpers
{
public:
    static bool StartsWith(const std::string& _string, const std::string& _prefix);
    static bool StartsWith(const std::string::const_iterator& _begin, const std::string::const_iterator& _end, const std::string& _prefix);

    static std::vector<std::string> Split(const std::string& _string, const std::string& _divider);
    static std::vector<std::string> Split(const std::string::const_iterator& _begin, const std::string::const_iterator & _end, const std::string& _divider);

    static std::string::const_iterator Find(const std::string::const_iterator &_begin, const std::string::const_iterator &_end, const std::string& _value);
};