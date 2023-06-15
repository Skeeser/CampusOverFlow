#pragma once
#include "logic.h"

class Course : public Logic
{
public:
    Course(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
    {
    }
    // ~User();

    // 用户管理
    void getCourse(char *input_data);
    void addCourse(char *input_data);
    void getCourseById(char *id);
    void getCourseByStuid(char *id);
    void getCourseByCollegeid(char *id);
    void putCourseById(char *id, char *input_data);
    void deleteCourseById(char *id);

private:
};