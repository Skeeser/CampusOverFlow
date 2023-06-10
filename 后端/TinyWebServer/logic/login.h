#pragma once
#include "logic.h"

class Login : public Logic
{
public:
    Login(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
    {
    }

    void login(char *input_data);

private:
};