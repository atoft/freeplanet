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

#include "StringHelpers.h"

#include <src/tools/globals.h>

bool StringHelpers::StartsWith(const std::string& _string, const std::string& _prefix)
{
    return _string.size() >= _prefix.size() && std::equal(_prefix.begin(), _prefix.end(), _string.begin());
}

bool StringHelpers::StartsWith(const std::string::const_iterator& _begin, const std::string::const_iterator& _end, const std::string& _prefix)
{
    return static_cast<std::size_t>(_end - _begin) >= _prefix.size() && std::equal(_prefix.begin(), _prefix.end(), _begin);
}

std::vector<std::string> StringHelpers::Split(const std::string& _string, const std::string& _divider)
{
    return Split(_string.begin(), _string.end(), _divider);
}

std::vector<std::string> StringHelpers::Split(const std::string::const_iterator &_begin, const std::string::const_iterator &_end, const std::string& _divider)
{
    std::vector<std::string> result;

    auto leftCursor = _begin;
    auto rightCursor = _begin;

    while (true)
    {
        if (static_cast<u32>(_end - rightCursor) >= _divider.size() && std::equal(_divider.begin(), _divider.end(), rightCursor))
        {
            // There is a matching divider at rightCursor

            result.emplace_back(leftCursor, rightCursor);

            leftCursor = rightCursor + _divider.size();
            rightCursor = leftCursor;
            assert(rightCursor <= _end);
        }

        if (rightCursor == _end)
        {
            if (leftCursor != rightCursor)
            {
                // We're at the end, include the last substring.
                // We might not get here if _string ends with _divider.
                result.emplace_back(leftCursor, rightCursor);
            }

            break;
        }
        else
        {
            ++rightCursor;
        }
    }

    return result;
}

std::string::const_iterator StringHelpers::Find(
        const std::string::const_iterator& _begin,
        const std::string::const_iterator& _end,
        const std::string &_value)
{
    auto it = _begin;

    while (it != _end)
    {
        if (StartsWith(it, _end, _value))
        {
            return it;
        }

        ++it;
    }

    return _end;
}
