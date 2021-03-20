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
#include <variant>
#include <vector>

#include <src/engine/inspection/BaseInspects.h>
#include <src/engine/inspection/contexts/InspectionContext.h>
#include <src/tools/globals.h>

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

using TestVariant = std::variant<TestInspectEnum, TestSubStruct>;

struct TestStruct
{
    u32 m_Property = 0;
    u32 m_OtherProperty = 3;
    TestSubStruct m_StructProperty;
    TestInspectEnum m_EnumProperty = TestInspectEnum::Apple;
    TestVariant m_Variant;
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

template <typename InspectionContext>
void Inspect(std::string _name, TestStructInVector& _target, InspectionContext& _context)
{
    constexpr u32 version = 0;

    // Explicitly ban missing values for this struct.
    _context.Struct(_name, InspectionType::TestStructInVector, version, InspectionStructRequirements::RequireExactMatch);

    Inspect("Boolean", _target.m_Boolean, _context);
    Inspect("Floaty", _target.m_Floaty, _context);
    Inspect("Fruit", _target.m_Fruit, _context);

    _context.EndStruct();
}

template <typename InspectionContext>
void Inspect(std::string _name, TestSubStruct& _target, InspectionContext& _context)
{
    constexpr u32 version = 0;
    _context.Struct(_name, InspectionType::TestSubStruct, version, InspectionStructRequirements::AllowMissingValues);

    Inspect("SubProperty", _target.m_SubProperty, _context);
    Inspect("IsTrue", _target.m_IsTrue, _context);

    Inspect("SeveralThings", _target.m_SeveralThings, _context);
    Inspect("NoThings", _target.m_NoThings, _context);
    Inspect("Amplitude", _target.m_Amplitude, _context);
    Inspect("VectorOfStructs", _target.m_VectorOfStructs, _context);

    _context.EndStruct();
}

template <typename InspectionContext>
void Inspect(std::string _name, TestStruct& _target, InspectionContext& _context)
{
    constexpr u32 version = 0;
    _context.Struct(_name, InspectionType::TestStruct, version);

    Inspect("Property", _target.m_Property, _context);
    Inspect("OtherProperty", _target.m_OtherProperty, _context);
    Inspect("StructProperty", _target.m_StructProperty, _context);
    Inspect("EnumProperty", _target.m_EnumProperty, _context);
    Inspect("Variant", _target.m_Variant, _context);

    _context.EndStruct();
};

template <typename InspectionContext>
void Inspect(std::string _name, TestPrimitiveOnlyStruct& _target, InspectionContext& _context)
{
    constexpr u32 version = 0;
    _context.Struct(_name, InspectionType::TestPrimitiveOnlyStruct, version);

    Inspect("Unsigned", _target.m_Unsigned, _context);
    Inspect("Signed", _target.m_Signed, _context);
    Inspect("Float", _target.m_Float, _context);
    Inspect("BoolA", _target.m_BoolA, _context);
    Inspect("BoolB", _target.m_BoolB, _context);

    _context.EndStruct();
};

