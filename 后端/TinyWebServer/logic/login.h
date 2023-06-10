#pragma once
#include "logic.h"

class Login : public Logic
{
public:
    Login(MYSQL *mysql, int close_log, int *len) : Logic(mysql, close_log, len)
    {
    }

    void login(char *input_data);

private:
};