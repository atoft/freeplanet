/*
 * Copyright 2017-2021 Alastair Toft
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
#include <filesystem>
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
    [[nodiscard]]
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
        
        Inspect("", _outValue, textContext);

        if (!textContext.m_ErrorMessage.empty())
        {
            LogError(textContext.m_ErrorMessage);
        }

        if (!textContext.m_WarningMessage.empty())
        {
            LogWarning(textContext.m_WarningMessage);
        }

        assert((textContext.m_Finished || textContext.m_Stack.empty()) && "Did you miss an EndStruct() call?");
        return textContext.m_Result;
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


        Inspect("", nonConstCopyOfSource, textContext);

        assert((textContext.m_Finished || textContext.m_Stack.empty()) && "Did you miss an EndStruct() call?");
    }

    template <typename T>
    [[nodiscard]]
    static FromBinaryInspectionResult FromBinary(const std::vector<u8>& _source, T& _outValue)
    {
        
            FromBinaryInspectionContext fromBinaryContext;
            fromBinaryContext.m_It = _source.begin();
            fromBinaryContext.m_Begin = _source.begin();
            fromBinaryContext.m_End = _source.end();        

        
        Inspect("", _outValue, fromBinaryContext);

        if (!fromBinaryContext.m_ErrorMessage.empty())
        {
            LogError(fromBinaryContext.m_ErrorMessage);
        }

        if (!fromBinaryContext.m_WarningMessage.empty())
        {
            LogWarning(fromBinaryContext.m_WarningMessage);
        }

        
        assert((fromBinaryContext.m_Finished || fromBinaryContext.m_Depth == 0) && "Did you miss an EndStruct() call?");
        return fromBinaryContext.m_Result;
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

        
        Inspect("", nonConstCopyOfSource, toBinaryContext);

        // TODO assert((finishedToBinaryContext.m_Finished) && "Did you miss an EndStruct() call?");
    }

    
    template <typename T>
    struct LoadFromTextResult
    {
        std::optional<T> m_Value;
        InspectionResult m_Result;
    };

    template <typename T>
    [[nodiscard]]
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
    struct LoadFromBinaryFileResult
    {
        std::optional<T> m_Value;
        FromBinaryInspectionResult m_Result;
    };

    template <typename T>
    [[nodiscard]]
    static LoadFromBinaryFileResult<T> LoadFromBinaryFile(std::string _path)
    {
        std::fstream infile;
        infile.open(_path);

        if (!infile)
        {
            LogError("File \"" + _path + "\" cannot be opened.");
            return { std::nullopt, FromBinaryInspectionResult::FileIOError };
        }

        const u64 expectedFileSize = std::filesystem::file_size(_path);
        
        std::vector<u8> input = std::vector<u8>(expectedFileSize);
        infile.read(reinterpret_cast<char*>(input.data()), expectedFileSize);

        if (static_cast<u64>(infile.gcount()) != expectedFileSize)
        {
            LogError("Read bytes from \"" + _path + "\" didn't match the reported filesize.");
            input.resize(static_cast<u64>(infile.gcount()));
        }

        infile.close();
        
        T value;
        FromBinaryInspectionResult result = FromBinary(input, value);

        if (result != FromBinaryInspectionResult::ReadSyntaxError)
        {
            return { value, result };
        }

        return { std::nullopt, result };
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
