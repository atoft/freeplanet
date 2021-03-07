/*
 * Copyright 2021 Alastair Toft
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

#include <cassert>
#include <variant>

class InspectionUtils
{
public:
    template<typename From, typename To>
    static To RawCast(const From& _from)
    {
        // TODO this can be replaced with a bit_cast in C++20
        return *((To*)(void*)&_from);
    };

    template<typename From, typename To>
    static To& RawCast(From& _from)
    {
        // TODO this can be replaced with a bit_cast in C++20
        return *((To*)(void*)&_from);
    };

    // Construct a std::variant from a runtime index.
    template <typename VariantType, std::size_t Index = 0>
    static VariantType VariantFromIndex(std::size_t _index)
    {
        if constexpr(Index >= std::variant_size_v<VariantType>)
        {
            return VariantType();
        }
        else
        {
            return _index == 0 ? VariantType{std::in_place_index<Index>} : InspectionUtils::VariantFromIndex<VariantType, Index + 1>(_index - 1);
        }
    }
};
