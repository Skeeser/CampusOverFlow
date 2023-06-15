#pragma once
#include "logic.h"

class Menus : public Logic
{
public:
    Menus(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
    {
    }
    // ~User();

    void getMenus(char *input_data);

private:
};