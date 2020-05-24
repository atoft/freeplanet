//
// Created by alastair on 19/01/2020.
//

#include "TestInspection.h"

#include <src/tests/TestHelpers.h>
#include <src/engine/inspection/TestStruct.h>
#include <src/engine/inspection/InspectionHelpers.h>

bool Test::TestInspection()
{
    bool result = true;

    result &= TestInspectionToFromText();
    result &= TestInspectionFromTextWithMissingValues();
    result &= TestInspectionFromTextWithExactMatchRequired();

    return result;
}

bool Test::TestInspectionToFromText()
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

    TestStruct fromString;
    const InspectionResult result = InspectionHelpers::FromText(toString, fromString);

    return TestResult(testValue == fromString && result == InspectionResult::Success);
}

bool Test::TestInspectionFromTextWithMissingValues()
{
    TestSubStruct subStruct;
    subStruct.m_Amplitude = 13.f;
    subStruct.m_IsTrue = true;
    subStruct.m_SubProperty = 64;
    subStruct.m_SeveralThings = { 4, 5, 6 };

    std::string toString = "";
    InspectionHelpers::ToText(subStruct, toString);

    const std::string toRemove = "IsTrue: true;";
    toString.replace(toString.find(toRemove), toRemove.length(), "");

    TestSubStruct fromString;
    const InspectionResult result = InspectionHelpers::FromText(toString, fromString);

    // Manually set the missing property.
    fromString.m_IsTrue = true;

    // With the missing property added, are the structs now the same?
    const bool areNonRemovedPropertiesCorrect = subStruct == fromString;

    return TestResult(result == InspectionResult::ReadIncomplete && areNonRemovedPropertiesCorrect);
}

bool Test::TestInspectionFromTextWithExactMatchRequired()
{
    TestStructInVector testStruct;

    std::string toString = "";
    InspectionHelpers::ToText(testStruct, toString);

    const std::string toRemove = "Floaty: 0.000000;";
    toString.replace(toString.find(toRemove), toRemove.length(), "");

    TestStructInVector fromString;
    const InspectionResult result = InspectionHelpers::FromText(toString, fromString);

    // This struct requires an exact match, so this should be an error.
    return TestResult(result == InspectionResult::ReadSyntaxError);
}
