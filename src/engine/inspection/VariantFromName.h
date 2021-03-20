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

#include <src/engine/inspection/contexts/GetTypeNameInspectionContext.h>

class InspectionVariants
{
public:
    // Construct a std::variant from a runtime name.
    // Performance will not be great because it requires constructing each possible variant type.
    // Variants seem to be the biggest pain for this whole InspectionContext idea.
    template <typename VariantType, std::size_t Index = 0>
    static VariantType VariantFromName(const std::string& _name, bool& _success)
    {
        if constexpr(Index >= std::variant_size_v<VariantType>)
        {
            _success = false;
            return VariantType();
        }
        else
        {
            VariantType potentialVariant = VariantType{std::in_place_index<Index>};
            
            GetTypeNameInspectionContext context;
            std::visit([&context](auto&& val) { Inspect("", val, context); }, potentialVariant);

            if (context.m_Result == _name)
            {
                _success = true;
                return potentialVariant;
            }
            else
            {
                return InspectionVariants::VariantFromName<VariantType, Index + 1>(_name, _success);
            }
        }
    }
};
