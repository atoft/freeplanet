//
// Created by alastair on 17/01/2020.
//

#pragma once

#include <string>
#include <vector>
#include <type_traits>

#include <src/tools/globals.h>
#include <src/engine/inspection/TypeInfo.h>
#include <src/tools/STL.h>
#include <src/tools/StringHelpers.h>

enum class InspectionResult
{
    Success,

    // Some properties were missing from the struct, but it was allowed.
    ReadIncomplete,

    ReadSyntaxError,

    FileIOError
};

enum class InspectionStructRequirements
{
    // The struct must always match the definition in its Inspect function.
    RequireExactMatch,

    // When reading from text, values are allowed to be missing from the struct, for example if the input is an older version.
    AllowMissingValues
};

class InspectionContext
{
    friend class InspectionHelpers;

public:
    void Struct(std::string _name, InspectionType _type, u32 _version, InspectionStructRequirements _requirements = InspectionStructRequirements::RequireExactMatch);
    void EndStruct();

    void U32(std::string _name, u32& _value);
    void S32(std::string _name, s32& _value);
    void F32(std::string _name, f32& _value);

    void Bool(std::string _name, bool& _value);

    template <typename EnumType, typename = std::enable_if_t<std::is_enum<EnumType>::value>>
    void Enum(std::string _name, EnumType& _value);

    template <typename ElementType>
    void Vector(std::string _name, std::vector<ElementType>& _value);

private:
    static InspectionType ToInspectionType(std::string _typeName);
    static std::string InspectionTypeToString(InspectionType _type);

    std::optional<std::string> ParseValueAndSkip(std::string _name, std::string::const_iterator& _it, std::string::const_iterator _end);
    void AppendNameAndValue(std::string _name, std::string _value);
    void AppendNewlineWithIndent();
    void SkipWhitespace(std::string::const_iterator& _it, std::string::const_iterator _end);
    bool SkipSingleToken(std::string::const_iterator& _it, std::string::const_iterator _end, const std::string& _token, bool _allowFailure = false);
    void AddError(std::string _error);
    void AddWarning(std::string _warning);
    u32 GetLineNumber() const;

    enum class Operation
    {
        FromText,
        ToText,
        FromBinary,
        ToBinary
    };

    static std::vector<TypeInfo> ms_TypeInfos;

    Operation m_Operation = Operation::ToText;

    struct InspectionStackEntry
    {
        // Container elements are different in text as they don't have associated names.
        bool m_InsideContainer = false;

        InspectionStructRequirements m_StructRequirements = InspectionStructRequirements::RequireExactMatch;
        bool m_SkipThisLevel = false;
    };

    // Maintain information about where in the struct we currently are.
    std::vector<InspectionStackEntry> m_Stack;

    bool m_Finished = false;
    InspectionResult m_Result = InspectionResult::Success;
    std::string m_ErrorMessage = "";
    std::string m_WarningMessage = "";

    // TODO placeholder.

    // ToText ///////////////////////////////
    std::string* m_TextBuffer;

    // FromText /////////////////////////////
    std::string::const_iterator m_TextBegin;
    std::string::const_iterator m_TextIt;
    std::string::const_iterator m_TextEnd;
};

template <typename EnumType, typename>
void InspectionContext::Enum(std::string _name, EnumType& _value)
{
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    switch (m_Operation)
    {
    case Operation::ToText:
    {
        AppendNameAndValue(_name, ToString(_value));

        break;
    }
    case Operation::FromText:
    {
        const std::optional<std::string> valueString = ParseValueAndSkip(_name, m_TextIt, m_TextEnd);

        if (!valueString)
        {
            return;
        }

        _value = STL::FromString<EnumType>(*valueString);

        break;
    }
    default:
        break;
    }
}

template <typename ElementType>
void InspectionContext::Vector(std::string _name, std::vector<ElementType>& _value)
{
    if (m_Finished || m_Stack.back().m_SkipThisLevel)
    {
        return;
    }

    switch (m_Operation)
    {
        case Operation::ToText:
        {
            AppendNewlineWithIndent();
            if (!m_Stack.back().m_InsideContainer)
            {
                *m_TextBuffer += _name + ": ";
            }

            *m_TextBuffer += "[";

            m_Stack.push_back({true, InspectionStructRequirements::RequireExactMatch});
            for (u32 elementIdx = 0; elementIdx < _value.size(); ++elementIdx)
            {
                Inspect(_name, _value[elementIdx], *this);

                if (elementIdx != _value.size() - 1)
                {
                    *m_TextBuffer += ", ";
                }
            }
            assert(m_Stack.back().m_InsideContainer);
            m_Stack.pop_back();

            *m_TextBuffer += "];";

            break;
        }
        case Operation::FromText:
        {
            if (!m_Stack.back().m_InsideContainer)
            {
                const std::string expectedPropertyIdentifier = _name + ":";
                assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, expectedPropertyIdentifier));
                m_TextIt += expectedPropertyIdentifier.size();
            }
            SkipWhitespace(m_TextIt, m_TextEnd);
            assert(StringHelpers::StartsWith(m_TextIt, m_TextEnd, "["));
            ++m_TextIt;
            SkipWhitespace(m_TextIt, m_TextEnd);

            m_Stack.push_back({true, InspectionStructRequirements::RequireExactMatch});

            if (!StringHelpers::StartsWith(m_TextIt, m_TextEnd, "]"))
            {
                while (!StringHelpers::StartsWith(m_TextIt, m_TextEnd, ";"))
                {
                    ElementType element;
                    Inspect(_name, element, *this);
                    _value.push_back(element);

                    SkipWhitespace(m_TextIt, m_TextEnd);
                    if (m_TextIt == m_TextEnd) return AddError("Unexpected end of file");
                }
            }
            else
            {
                // Skip the empty list.
                SkipSingleToken(m_TextIt, m_TextEnd, "]");
            }

            assert(m_Stack.back().m_InsideContainer);
            m_Stack.pop_back();

            SkipSingleToken(m_TextIt, m_TextEnd, ";");
            SkipWhitespace(m_TextIt, m_TextEnd);

            break;
        }
        default:
            break;
    }
}
