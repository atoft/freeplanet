//
// Created by alastair on 03/12/19.
//

#pragma once

#include <string>
#include <vector>

class StringHelpers
{
public:
    static bool StartsWith(const std::string& _string, const std::string& _prefix);
    static bool StartsWith(const std::string::const_iterator& _begin, const std::string::const_iterator& _end, const std::string& _prefix);

    static std::vector<std::string> Split(const std::string& _string, const std::string& _divider);
    static std::vector<std::string> Split(const std::string::const_iterator& _begin, const std::string::const_iterator & _end, const std::string& _divider);

    static std::string::const_iterator Find(const std::string::const_iterator &_begin, const std::string::const_iterator &_end, const std::string& _value);
};