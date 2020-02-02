//
// Created by alastair on 20/01/2020.
//

#include "TestStruct.h"

#include <src/engine/inspection/BaseInspects.h>
#include <src/engine/inspection/InspectionContext.h>

void Inspect(std::string _name, TestSubStruct& _target, InspectionContext& _context)
{
    constexpr u32 version = 0;
    _context.Struct(_name, InspectionType::TestSubStruct, version);

    Inspect("SubProperty", _target.m_SubProperty, _context);

    // TODO implement these inspections
    //Inspect("IsTrue", _target.m_IsTrue, _context);
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