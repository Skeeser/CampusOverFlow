#pragma once

#include "logic.h"


class College : public Logic
{
public:
    College(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
    {
    }
    // ~User();

    void getColleges();

private:
};