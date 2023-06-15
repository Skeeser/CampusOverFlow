#pragma once

#include <exception>
#include <string>

class MyException : public std::exception
{
public:
    explicit MyException(const std::string &message) : message_(message) {}

    // 重写 what() 函数来提供异常描述
    const char *what() const noexcept override
    {
        return message_.c_str();
    }

private:
    std::string message_;
};
