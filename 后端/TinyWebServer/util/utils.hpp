#pragma once
#include <string>
#include <vector>

class Utils
{
public:
    static std::vector<std::string> split(std::string str, std::string delimiter)
    {
        std::vector<std::string> vec;
        size_t len = str.length();
        size_t pos = str.find(delimiter);
        while (pos != -1)
        {
            std::string substr = str.substr(0, pos);
            str = str.substr(pos + 1, len - pos - 1);
            len = str.length();
            pos = str.find(delimiter);
            vec.push_back(substr);
        }
        vec.push_back(str);
        return vec;
    }
};