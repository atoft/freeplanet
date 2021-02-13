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

#include <memory>
#include <vector>

#include <src/tools/globals.h>

class InspectionContext;

enum class TestInspectEnum
{
    Apple,
    Banana,
    Carrot,
    Count
};

std::string ToString(TestInspectEnum _enum);

struct TestStructInVector
{
    bool m_Boolean = true;
    f32 m_Floaty = 0.f;
    std::vector<TestInspectEnum> m_Fruit;

    bool operator==(const TestStructInVector& _other) const;
};

struct TestSubStruct
{
    u32 m_SubProperty = 0;
    bool m_IsTrue = false;
    std::vector<u32> m_SeveralThings;
    std::vector<u32> m_NoThings;
    f32 m_Amplitude = 0.f;
    std::vector<TestStructInVector> m_VectorOfStructs;
    bool operator==(const TestSubStruct& _other) const;
};

struct TestStruct
{
    u32 m_Property = 0;
    u32 m_OtherProperty = 3;
    TestSubStruct m_StructProperty;
    TestInspectEnum m_EnumProperty = TestInspectEnum::Apple;
    bool operator==(const TestStruct& _other) const;
};

struct TestPrimitiveOnlyStruct
{
    u32 m_Unsigned = 0;
    s32 m_Signed = 0;
    f32 m_Float = 0.f;
    bool m_BoolA = false;
    bool m_BoolB = false;
    bool operator==(const TestPrimitiveOnlyStruct& _other) const;
};

void Inspect(std::string _name, TestStructInVector& _target, InspectionContext& _context);
void Inspect(std::string _name, TestSubStruct& _target, InspectionContext& _context);
void Inspect(std::string _name, TestStruct& _target, InspectionContext& _context);
void Inspect(std::string _name, TestPrimitiveOnlyStruct& _target, InspectionContext& _context);
