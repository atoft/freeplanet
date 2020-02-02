//
// Created by alastair on 19/01/2020.
//

#include "TestInspection.h"

#include <src/engine/inspection/TestStruct.h>
#include <src/engine/inspection/InspectionHelpers.h>

bool Test::TestInspection()
{
    TestStruct testValue;
    testValue.m_Property = 7;
    testValue.m_OtherProperty = 47;
    testValue.m_StructProperty.m_SubProperty = 123;

    std::string toString = "";
    InspectionHelpers::ToText(testValue, toString);

    LogMessage(toString);

    TestStruct fromString;
    InspectionHelpers::FromText(toString, fromString);

    const bool success = testValue.m_Property == fromString.m_Property
            && testValue.m_OtherProperty == fromString.m_OtherProperty
            && testValue.m_StructProperty.m_SubProperty == fromString.m_StructProperty.m_SubProperty;

    if (success)
    {
        LogMessage("PASS TestInpsection");
    }
    else
    {
        LogError("FAIL TestInspection");
    }

    return success;
}