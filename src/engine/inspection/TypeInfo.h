//
// Created by alastair on 17/01/2020.
//

#pragma once

#include <memory>

enum class InspectionType
{
    Invalid,

    vec2,
    vec3,
    ivec2,
    ivec3,

    TestStruct,
    TestSubStruct,

    EngineConfig,
};

class TypeInfo
{
public:
    std::string m_Name;
    InspectionType m_Type;
};
