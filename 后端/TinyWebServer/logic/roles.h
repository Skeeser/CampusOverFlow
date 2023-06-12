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
    void giveRole(char *id, char *input_data);
    void addRoles(char* input_data);
    void getRoleById(char* id);

private:
    // 存储的哈希表，存储每个权限人拥有的所有权限
    std::unordered_map<int, std::vector<std::string>> roles_map;

    void getAllRoleList(Json::Value &ret_root);

    
};