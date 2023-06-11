#pragma once
#include "logic.h"
#include "unordered_map"

class Roles : public Logic
{

public:
    Roles(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
    {
    }

    void getRoles();

private:
    // 存储的哈希表，存储每个权限人拥有的所有权限
    std::unordered_map<int, std::vector<std::string>> roles_map;

    void getAllRoleList(Json::Value &ret_root);

    // void addRoles();
};