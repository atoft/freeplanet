//
// Created by alastair on 17/01/2020.
//

#pragma once

#include <memory>
#include <vector>

#include <src/tools/globals.h>

class InspectionContext;

struct TestSubStruct
{
    u32 m_SubProperty = 0;
    bool m_IsTrue = false;
    std::vector<u32> m_SeveralThings;
};

struct TestStruct
{
    u32 m_Property = 0;
    u32 m_OtherProperty = 3;
    TestSubStruct m_StructProperty;

    bool operator==(const TestStruct& _other) const;
};

void Inspect(std::string _name, TestSubStruct& _target, InspectionContext& _context);
void Inspect(std::string _name, TestStruct& _target, InspectionContext& _context);
