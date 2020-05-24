//
// Created by alastair on 19/01/2020.
//

#pragma once

#include <optional>
#include <fstream>
#include <string>
#include <sstream>

#include <src/engine/inspection/InspectionContext.h>

class InspectionHelpers
{
public:
    template <typename T>
    static InspectionResult FromText(const std::string& _source, T& _outValue)
    {
        // TODO handle deprecation here. If the version number from the input is old,
        // call a deprecate method. It could read into a "deprecated version" of the struct
        // which the caller can then use to populate the newer version.

        InspectionContext inspectionContext;
        inspectionContext.m_TextIt = _source.begin();
        inspectionContext.m_TextBegin = _source.begin();
        inspectionContext.m_TextEnd = _source.end();
        inspectionContext.m_Operation = InspectionContext::Operation::FromText;
        Inspect("", _outValue, inspectionContext);

        if (!inspectionContext.m_ErrorMessage.empty())
        {
            LogError(inspectionContext.m_ErrorMessage);
        }

        if (!inspectionContext.m_WarningMessage.empty())
        {
            LogWarning(inspectionContext.m_WarningMessage);
        }

        assert((inspectionContext.m_Finished || inspectionContext.m_Stack.empty()) && "Did you miss an EndStruct() call?");
        return inspectionContext.m_Result;
    }

    template <typename T>
    static void ToText(const T& _source, std::string& _outText)
    {
        // The Inspect methods must have a non-const ref so that the struct
        // can be written to in FromText/FromBinary operations. For const correctness, make a
        // copy here. If this becomes slow, can do a const-cast to avoid the copy.
        T nonConstCopyOfSource = _source;

        InspectionContext inspectionContext;
        inspectionContext.m_TextBuffer = &_outText;
        inspectionContext.m_Operation = InspectionContext::Operation::ToText;
        Inspect("", nonConstCopyOfSource, inspectionContext);
        assert((inspectionContext.m_Finished || inspectionContext.m_Stack.empty()) && "Did you miss an EndStruct() call?");
    }

    template <typename T>
    struct LoadFromTextResult
    {
        std::optional<T> m_Value;
        InspectionResult m_Result;
    };

    template <typename T>
    static LoadFromTextResult<T> LoadFromText(std::string _path)
    {
        std::fstream infile;
        infile.open(_path);

        if (!infile)
        {
            LogError("File \"" + _path + "\" cannot be opened.");
            return { std::nullopt, InspectionResult::FileIOError };
        }

        std::stringstream input;
        input << infile.rdbuf();

        T value;
        InspectionResult result = FromText(input.str(), value);

        infile.close();

        if (result != InspectionResult::ReadSyntaxError)
        {
            return { value, result };
        }

        return { std::nullopt, result };
    }

    template <typename T>
    static void SaveToText(const T& _source, std::string _path)
    {
        std::fstream outfile;
        outfile.open(_path, std::fstream::out);

        if (!outfile)
        {
            LogError("File \"" + _path + "\" cannot be opened.");
            return;
        }

        std::string output;
        ToText(_source, output);

        outfile << output;

        outfile.flush();
        outfile.close();
    }
};
