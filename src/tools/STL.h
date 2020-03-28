//
// Created by alastair on 28/03/2020.
//

#pragma once

#include <algorithm>

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

        _container.erase(iterator);
        return true;
    }
};