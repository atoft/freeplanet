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

#include <algorithm>

#include <src/tools/globals.h>

// Helpers for the standard template library.
class STL
{
public:
    // Remove all elements equal to _value from _container (with resize) and return true if at least one was removed.
    template<typename Container, typename ValueType>
    static bool Remove(Container& _container, const ValueType& _value)
    {
        if (_container.empty())
        {
            return false;
        }

        auto iterator = std::remove(_container.begin(), _container.end(), _value);

        if (iterator == _container.end())
        {
            return false;
        }

        _container.erase(iterator, _container.end());
        return true;
    }

    // Remove all elements matching _predicate from _container (with resize) and return true if at least one was removed.
    template<typename Container, typename Predicate>
    static bool RemoveIf(Container& _container, Predicate _predicate)
    {
        if (_container.empty())
        {
            return false;
        }

        auto iterator = std::remove_if(_container.begin(), _container.end(), _predicate);

        if (iterator == _container.end())
        {
            return false;
        }

        _container.erase(iterator, _container.end());
        return true;
    }
    
    template <typename EnumType>
    static EnumType FromString(std::string _valueString)
    {
        for (u32 enumIdx = 0; enumIdx < static_cast<u32>(EnumType::Count); ++enumIdx)
        {
            if (ToString(static_cast<EnumType>(enumIdx)) == _valueString)
            {
                return static_cast<EnumType>(enumIdx);
            }
        }

        return EnumType::Count;
    }

    template<typename Container>
    static void Append(Container& _container, const Container& _toAppend)
    {
        _container.insert(std::end(_container), std::begin(_toAppend), std::end(_toAppend));
    }
};
