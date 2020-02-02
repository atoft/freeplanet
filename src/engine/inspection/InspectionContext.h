//
// Created by alastair on 17/01/2020.
//

#pragma once

#include <string>
#include <vector>

#include <src/tools/globals.h>
#include <src/engine/inspection/TypeInfo.h>

class InspectionContext
{
    friend class InspectionHelpers;

public:
    void Struct(std::string _name, InspectionType _type, u32 _version);
    void EndStruct();

    void U32(std::string _name, u32& _value);
    void S32(std::string _name, s32& _value);
    void F32(std::string _name, f32& _value);

    void Bool(std::string _name, bool& _value);

private:
    static InspectionType ToInspectionType(std::string _typeName);
    static std::string ToString(InspectionType _type);

    enum class Operation
    {
        FromText,
        ToText,
        FromBinary,
        ToBinary
    };

    static std::vector<TypeInfo> ms_TypeInfos;

    Operation m_Operation = Operation::ToText;
    
    // How many nested structs are we in.
    u32 m_Depth = 0;

    // TODO placeholder.

    // ToText ///////////////////////////////
    std::string* m_TextBuffer;

    // FromText /////////////////////////////
    std::string::const_iterator m_TextIt;
    std::string::const_iterator m_TextEnd;
};
