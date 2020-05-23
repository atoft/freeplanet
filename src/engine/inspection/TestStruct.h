//
// Created by alastair on 17/01/2020.
//

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
};

struct TestStruct
{
    u32 m_Property = 0;
    u32 m_OtherProperty = 3;
    TestSubStruct m_StructProperty;
    TestInspectEnum m_EnumProperty = TestInspectEnum::Apple;
    bool operator==(const TestStruct& _other) const;
};

void Inspect(std::string _name, TestStructInVector& _target, InspectionContext& _context);
void Inspect(std::string _name, TestSubStruct& _target, InspectionContext& _context);
void Inspect(std::string _name, TestStruct& _target, InspectionContext& _context);
