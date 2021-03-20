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

#include "TestStruct.h"

#include <src/engine/inspection/BaseInspects.h>
#include <src/engine/inspection/contexts/InspectionContext.h>
#include <src/engine/inspection/VectorInspect.h>

bool TestSubStruct::operator==(const TestSubStruct& _other) const
{
    return m_SubProperty == _other.m_SubProperty
           && m_IsTrue == _other.m_IsTrue
           && std::equal(m_SeveralThings.begin(), m_SeveralThings.end(), _other.m_SeveralThings.begin())
           && m_Amplitude == _other.m_Amplitude
           && std::equal(m_VectorOfStructs.begin(), m_VectorOfStructs.end(), _other.m_VectorOfStructs.begin())
            ;
}

bool TestStruct::operator==(const TestStruct& _other) const
{
    return m_Property == _other.m_Property
        && m_OtherProperty == _other.m_OtherProperty
        && m_StructProperty == _other.m_StructProperty
        && m_EnumProperty == _other.m_EnumProperty
        && m_Variant == _other.m_Variant
        ;
}

bool TestStructInVector::operator==(const TestStructInVector& _other) const
{
    return m_Boolean == _other.m_Boolean
        && m_Floaty == _other.m_Floaty
        && m_Fruit == _other.m_Fruit
        ;
}

bool TestPrimitiveOnlyStruct::operator==(const TestPrimitiveOnlyStruct& _other) const
{
    return m_Unsigned == _other.m_Unsigned
        && m_Signed == _other.m_Signed
        && m_Float == _other.m_Float
        && m_BoolA == _other.m_BoolA
        && m_BoolB == _other.m_BoolB;
}

std::string ToString(TestInspectEnum _enum)
{
    switch (_enum)
    {
        case TestInspectEnum::Apple:
            return "Apple";
        case TestInspectEnum::Banana:
            return "Banana";
        case TestInspectEnum::Carrot:
            return "Carrot";
        default:
            break;
    }
    return "";
}
