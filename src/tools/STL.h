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