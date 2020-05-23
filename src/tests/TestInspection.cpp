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
    testValue.m_StructProperty.m_SeveralThings = {88, 4, 5, 22};
    testValue.m_StructProperty.m_Amplitude = 15.f;
    testValue.m_StructProperty.m_VectorOfStructs.push_back({true, -1.f, {TestInspectEnum::Banana, TestInspectEnum::Carrot}});
    testValue.m_StructProperty.m_VectorOfStructs.push_back({false, 2.f, {TestInspectEnum::Apple}});
    testValue.m_EnumProperty = TestInspectEnum::Banana;

    std::string toString = "";
    InspectionHelpers::ToText(testValue, toString);

    LogMessage(toString);

    TestStruct fromString;
    InspectionHelpers::FromText(toString, fromString);

    return TestResult(testValue == fromString);
}