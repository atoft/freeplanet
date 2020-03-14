//
// Created by alastair on 20/01/2020.
//

#include "TestStruct.h"

#include <src/engine/inspection/BaseInspects.h>
#include <src/engine/inspection/InspectionContext.h>

bool TestStruct::operator==(const TestStruct& _other) const
{
    return m_Property == _other.m_Property
        && m_OtherProperty == _other.m_OtherProperty
        && m_StructProperty.m_SubProperty == _other.m_StructProperty.m_SubProperty
        && m_StructProperty.m_IsTrue == _other.m_StructProperty.m_IsTrue;
}

void Inspect(std::string _name, TestSubStruct& _target, InspectionContext& _context)
{
    constexpr u32 version = 0;
    _context.Struct(_name, InspectionType::TestSubStruct, version);

    Inspect("SubProperty", _target.m_SubProperty, _context);
    Inspect("IsTrue", _target.m_IsTrue, _context);

    // TODO implement these inspections
    //Inspect("SeveralThings", _target.m_SeveralThings, _context);

    _context.EndStruct();
}

void Inspect(std::string _name, TestStruct& _target, InspectionContext& _context)
{
    constexpr u32 version = 0;
    _context.Struct(_name, InspectionType::TestStruct, version);

    Inspect("Property", _target.m_Property, _context);
    Inspect("OtherProperty", _target.m_OtherProperty, _context);
    Inspect("StructProperty", _target.m_StructProperty, _context);

    _context.EndStruct();
};