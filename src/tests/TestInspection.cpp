//
// Created by alastair on 19/01/2020.
//

#include "TestInspection.h"

#include <src/tests/TestHelpers.h>
#include <src/engine/inspection/TestStruct.h>
#include <src/engine/inspection/InspectionHelpers.h>

bool Test::TestInspection()
{
    TestStruct testValue;
    testValue.m_Property = 7;
    testValue.m_OtherProperty = 47;
    testValue.m_StructProperty.m_SubProperty = 123;
    testValue.m_StructProperty.m_IsTrue = true;

    std::string toString = "";
    InspectionHelpers::ToText(testValue, toString);

    LogMessage(toString);

    TestStruct fromString;
    InspectionHelpers::FromText(toString, fromString);

    return TestResult(testValue == fromString);
}