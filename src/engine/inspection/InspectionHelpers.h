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

#pragma once

#include <optional>
#include <fstream>
#include <string>
#include <sstream>

#include <src/engine/inspection/contexts/FromBinaryInspectionContext.h>
#include <src/engine/inspection/contexts/ToBinaryInspectionContext.h>
#include <src/engine/inspection/contexts/InspectionContext.h>
#include <src/engine/inspection/contexts/TextInspectionContext.h>

class InspectionHelpers
{
public:
    template <typename T>
    static InspectionResult FromText(const std::string& _source, T& _outValue)
    {
        // TODO handle deprecation here. If the version number from the input is old,
        // call a deprecate method. It could read into a "deprecated version" of the struct
        // which the caller can then use to populate the newer version.

        TextInspectionContext textContext;
        textContext.m_TextIt = _source.begin();
        textContext.m_TextBegin = _source.begin();
        textContext.m_TextEnd = _source.end();
        textContext.m_Operation = TextInspectionContext::Operation::FromText;

        InspectionContext inspectionContext;
        textContext.m_Outer = &inspectionContext;
        inspectionContext.m_Variant = textContext;
        
        Inspect("", _outValue, inspectionContext);

        if (!std::get<TextInspectionContext>(inspectionContext.m_Variant).m_ErrorMessage.empty())
        {
            LogError(std::get<TextInspectionContext>(inspectionContext.m_Variant).m_ErrorMessage);
        }

        if (!std::get<TextInspectionContext>(inspectionContext.m_Variant).m_WarningMessage.empty())
        {
            LogWarning(std::get<TextInspectionContext>(inspectionContext.m_Variant).m_WarningMessage);
        }

        assert((std::get<TextInspectionContext>(inspectionContext.m_Variant).m_Finished || std::get<TextInspectionContext>(inspectionContext.m_Variant).m_Stack.empty()) && "Did you miss an EndStruct() call?");
        return std::get<TextInspectionContext>(inspectionContext.m_Variant).m_Result;
    }

    template <typename T>
    static void ToText(const T& _source, std::string& _outText)
    {
        // The Inspect methods must have a non-const ref so that the struct
        // can be written to in FromText/FromBinary operations. For const correctness, make a
        // copy here. If this becomes slow, can do a const-cast to avoid the copy.
        T nonConstCopyOfSource = _source;

        TextInspectionContext textContext;
        textContext.m_TextBuffer = &_outText;
        textContext.m_Operation = TextInspectionContext::Operation::ToText;

        InspectionContext inspectionContext;
        textContext.m_Outer = &inspectionContext;
        inspectionContext.m_Variant = textContext;

        Inspect("", nonConstCopyOfSource, inspectionContext);
        assert((std::get<TextInspectionContext>(inspectionContext.m_Variant).m_Finished || std::get<TextInspectionContext>(inspectionContext.m_Variant).m_Stack.empty()) && "Did you miss an EndStruct() call?");
    }

    template <typename T>
    static void FromBinary(const std::vector<u8>& _source, T& _outValue)
    {
        FromBinaryInspectionContext fromBinaryContext;
        fromBinaryContext.m_It = _source.begin();
        fromBinaryContext.m_Begin = _source.begin();
        fromBinaryContext.m_End = _source.end();        

        InspectionContext inspectionContext;
        fromBinaryContext.m_Outer = &inspectionContext;
        inspectionContext.m_Variant = fromBinaryContext;
        
        Inspect("", _outValue, inspectionContext);
        assert((std::get<FromBinaryInspectionContext>(inspectionContext.m_Variant).m_Finished || std::get<FromBinaryInspectionContext>(inspectionContext.m_Variant).m_Depth == 0) && "Did you miss an EndStruct() call?");
    }

    
    template <typename T>
    static void ToBinary(const T& _source, std::vector<u8>& _outData)
    {
        // The Inspect methods must have a non-const ref so that the struct
        // can be written to in FromText/FromBinary operations. For const correctness, make a
        // copy here. If this becomes slow, can do a const-cast to avoid the copy.
        T nonConstCopyOfSource = _source;

        ToBinaryInspectionContext toBinaryContext;
        toBinaryContext.m_Buffer = &_outData;

        InspectionContext inspectionContext;
        toBinaryContext.m_Outer = &inspectionContext;
        inspectionContext.m_Variant = toBinaryContext;
        
        Inspect("", nonConstCopyOfSource, inspectionContext);
        //assert((std::get<TextInspectionContext>(inspectionContext.m_Variant).m_Finished || std::get<TextInspectionContext>(inspectionContext.m_Variant).m_Stack.empty()) && "Did you miss an EndStruct() call?");
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
    
    template <typename T>
    static void SaveToBinaryFile(const T& _source, std::string _path)
    {
        std::fstream outfile;
        outfile.open(_path, std::fstream::out);

        if (!outfile)
        {
            LogError("File \"" + _path + "\" cannot be opened.");
            return;
        }

        std::vector<u8> output;
        ToBinary(_source, output);

        outfile.write(reinterpret_cast<char*>(output.data()), output.size());

        outfile.flush();
        outfile.close();
    }
};
