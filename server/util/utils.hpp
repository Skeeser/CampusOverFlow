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

    // URL解码函数
    static std::string urlDecode(const std::string &encoded)
    {
        std::ostringstream decoded;
        for (std::size_t i = 0; i < encoded.length(); ++i)
        {
            if (encoded[i] == '%')
            {
                std::string hex;
                hex += encoded[++i];
                hex += encoded[++i];
                int value;
                std::istringstream(hex) >> std::hex >> value;
                decoded << static_cast<char>(value);
            }
            else if (encoded[i] == '+')
            {
                decoded << ' ';
            }
            else
            {
                decoded << encoded[i];
            }
        }
        return decoded.str();
    }
};