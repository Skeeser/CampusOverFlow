#include "roles.h"
#include "../util/utils.hpp"

// 获取权限列表
void Roles::getAllRoleList(Json::Value &ret_root)
{

    Json::Value data;
    Json::Value temp;

    std::string sql_string("SELECT * FROM sp_role ;");

    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    // 在获取前先清除
    clearTableKey();
    getTableKey("sp_role");

    int ret = mysql_query(mysql_, sql_string.c_str());

    if (!ret) // 查询成功
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);

        while (MYSQL_ROW row = mysql_fetch_row(result))
        {

            temp["id"] = row[indexOf("role_id")];
            temp["roleName"] = row[indexOf("role_name")];
            temp["roleDesc"] = row[indexOf("role_desc")];
            auto ids = Utils::split(row[indexOf("ps_ids")], ",");
            roles_map[stoi(row[indexOf("role_id")])] = ids;
            data.append(temp);
            temp.clear();
        }
        ret_root["data"] = data;
    }
    else
    {
        errorLogic(404, "查询失败");
        return;
    }
}

void Roles::getRoles()
{

    Json::Value ret_root;
    Json::Value data;
    Json::Value temp;
    Json::Value meta;

    // 先验证token
    if (!is_token_vaild_)
    {
        errorLogic(403, "token验证不通过");
        return;
    }

    // 先获取每个role拥有的所有权限
    getAllRoleList(ret_root);

    std::string sql_string("SELECT * FROM sp_permission_api as api LEFT JOIN sp_permission as main ON main.ps_id = api.ps_id WHERE main.ps_id is not null;");

    int level0_num = 0;
    int mg_id = -1;
    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    // std::shared_ptr<std::map<int, Json::Value>> level1_data = std::make_shared<std::map<int, Json::Value>>();
    std::unordered_map<int, MYSQL_ROW> rows;
    std::unordered_map<int, int> child_par_ids;
    // 在获取前先清除
    clearTableKey();
    getTableKey("sp_permission_api");
    getTableKey("sp_permission");

    int ret = mysql_query(mysql_, sql_string.c_str());
    if (!ret) // 查询成功
    {

        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);

        while (MYSQL_ROW row = mysql_fetch_row(result))
        {
            rows[std::stoi(row[indexOf("ps_id")])] = row;
        }

        // todo 三重循环，怎么优化？
        // 遍历ret_root中data
        for (Json::Value &json_value : ret_root["data"])
        {
            int id = stoi(json_value["id"].asString().c_str());
            auto vec = roles_map[id];
            LOG_DEBUG("getAllRoles%d", id);
            for (auto ps_id : vec)
            {
                int pid = std::stoi(ps_id);
                auto row = rows[pid];
                // 处理0级权限
                if (strncasecmp(row[indexOf("ps_level")], "0", 1) == 0)
                {
                    temp["id"] = pid;
                    temp["authName"] = row[indexOf("ps_name")];
                    temp["path"] = row[indexOf("ps_api_path")];
                    // temp["pid"] = row[indexOf("ps_pid")];
                    child_par_ids[pid] = std::stoi(row[indexOf("ps_pid")]);
                    // temp["children"] = {}; // debug: 要用[]?
                    json_value["children"].append(temp);
                    temp.clear();
                }
            }

            for (auto ps_id : vec)
            {
                int pid = std::stoi(ps_id);
                auto row = rows[pid];
                // 处理1级权限
                if (strncasecmp(row[indexOf("ps_level")], "1", 1) == 0)
                {
                    temp["id"] = pid;
                    temp["authName"] = row[indexOf("ps_name")];
                    temp["path"] = row[indexOf("ps_api_path")];
                    auto parent_id = row[indexOf("ps_pid")];
                    // temp["pid"] = parent_id;
                    child_par_ids[pid] = std::stoi(parent_id);
                    auto level1_json = findChildrenJsonByMember(json_value["children"], "id", parent_id);
                    level1_json["children"].append(temp);
                    temp.clear();
                }
            }
            LOG_DEBUG("getAllRoles5");
            for (auto ps_id : vec)
            {
                int pid = std::stoi(ps_id);
                auto row = rows[pid];
                // 处理1级权限
                if (strncasecmp(row[indexOf("ps_level")], "2", 1) == 0)
                {
                    temp["id"] = pid;
                    temp["authName"] = row[indexOf("ps_name")];
                    temp["path"] = row[indexOf("ps_api_path")];
                    auto parent_id = row[indexOf("ps_pid")];
                    temp["pid"] = parent_id;
                    auto level2_json = findChildrenJsonByMember(findChildrenJsonByMember(json_value["children"], "id", std::to_string(child_par_ids[std::stoi(parent_id)])), "id", parent_id);
                    level2_json["children"].append(temp);
                    temp.clear();
                }
            }
        }
        LOG_DEBUG("getAllRoles6");
        meta["msg"] = "登录成功";
        meta["status"] = 200;
        ret_root["meta"] = meta;
        LOG_DEBUG("getAllRoles4");
    }
    else
    {
        errorLogic(404, "获取目录列表失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}