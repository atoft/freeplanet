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

#include "TestInspection.h"
#include "src/engine/inspection/InspectionTypes.h"
#include "src/engine/inspection/contexts/FromBinaryInspectionContext.h"
#include "src/tools/globals.h"

#include <src/tests/TestHelpers.h>
#include <src/engine/inspection/TestStruct.h>
#include <src/engine/inspection/InspectionHelpers.h>

bool Test::TestInspection()
{
    bool result = true;

    result &= TestInspectionToFromText();
    result &= TestInspectionFromTextWithMissingValues();
    result &= TestInspectionFromTextWithExactMatchRequired();

    result &= TestInspectionToFromBinaryPrimitives();
    result &= TestInspectionToFromBinaryComplex();
    result &= TestInspectionFromBinaryEmpty();
    result &= TestInspectionFromBinaryTooShort();
    result &= TestInspectionFromBinaryTooLong();
    
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
    testValue.m_Variant = TestInspectEnum::Carrot;

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

bool Test::TestInspectionToFromBinaryPrimitives()
{
    TestPrimitiveOnlyStruct structA;
    structA.m_Unsigned = 4294967294;
    structA.m_Signed = -1;
    structA.m_Float = 32.5f;
    structA.m_BoolA = true;

    std::vector<u8> binaryData;
    InspectionHelpers::ToBinary(structA, binaryData);
    
    TestPrimitiveOnlyStruct fromBinary;
    const FromBinaryInspectionResult result = InspectionHelpers::FromBinary(binaryData, fromBinary);

    return TestResult(result == FromBinaryInspectionResult::Success && structA == fromBinary);
}

bool Test::TestInspectionToFromBinaryComplex()
{
    TestStruct testValue;
    testValue.m_Property = 9;
    testValue.m_OtherProperty = 65535;
    testValue.m_StructProperty.m_SubProperty = 321;
    testValue.m_StructProperty.m_IsTrue = true;
    testValue.m_StructProperty.m_SeveralThings = {88, 4, 5, 22};
    testValue.m_StructProperty.m_Amplitude = -7.f;
    testValue.m_StructProperty.m_VectorOfStructs.push_back({true, -3.f, {TestInspectEnum::Banana, TestInspectEnum::Carrot}});
    testValue.m_StructProperty.m_VectorOfStructs.push_back({false, 4.f, {TestInspectEnum::Apple}});
    testValue.m_EnumProperty = TestInspectEnum::Carrot;

    TestSubStruct structInsideVariant;
    structInsideVariant.m_SeveralThings = {1, 2, 3, 4};
    structInsideVariant.m_Amplitude = -2.f;
    testValue.m_Variant = structInsideVariant;
    
    std::vector<u8> binaryData;
    InspectionHelpers::ToBinary(testValue, binaryData);

    TestStruct fromBinary;
    const FromBinaryInspectionResult result = InspectionHelpers::FromBinary(binaryData, fromBinary);

    return TestResult(result == FromBinaryInspectionResult::Success && testValue == fromBinary);
}

bool Test::TestInspectionFromBinaryEmpty()
{
    std::vector<u8> data = {};

    TestStruct testStruct;
    const FromBinaryInspectionResult result = InspectionHelpers::FromBinary(data, testStruct);

    return TestResult(result == FromBinaryInspectionResult::ReadSyntaxError);
}

bool Test::TestInspectionFromBinaryTooShort()
{
    std::vector<u8> data = {0,0,0,5,0,0,0,0,0,0,0};
    
    TestStruct testStruct;
    const FromBinaryInspectionResult result = InspectionHelpers::FromBinary(data, testStruct);

    // TODO return value from inspection.
    return TestResult(result == FromBinaryInspectionResult::ReadSyntaxError);
}

bool Test::TestInspectionFromBinaryTooLong()
{
    TestStruct testValue;

    std::vector<u8> binaryData;
    InspectionHelpers::ToBinary(testValue, binaryData);

    binaryData.push_back(0);
    binaryData.push_back(255);
    
    TestStruct testStruct;
    const FromBinaryInspectionResult result = InspectionHelpers::FromBinary(binaryData, testStruct);

    return TestResult(result == FromBinaryInspectionResult::ReadSyntaxError);
}

