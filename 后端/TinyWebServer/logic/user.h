#pragma once
#include "logic.h"

class User : public Logic
{
public:
    User(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
    {
    }
    // ~User();

    // 用户管理
    void getUsers(char *input_data);
    void addUser(char *input_data);
    void getUserById(char *id);
    void putUserById(char *id, char *input_data);
    void deleteUserById(char *id);

private:
};