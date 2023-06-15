#pragma once

#include "logic.h"

class Class : public Logic
{
public:
    Class(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
    {
    }
    void getClass(char *input_data);
    void addClass(char *input_data);
    void getClassById(char *id);
    void putClassById(char *id, char *input_data);
    void putCourseToClass(char *id, char *input_data);
    void deleteClassById(char *id);

private:
};