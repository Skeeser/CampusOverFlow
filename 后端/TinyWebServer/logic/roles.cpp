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

            // LOG_DEBUG("getAllRoles%d", id);
            for (auto ps_id : vec)
            {
                if (ps_id == "")
                    continue;
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
                if (ps_id == "")
                    continue;
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
                    auto &level1_json = findChildrenJsonByMember(json_value["children"], "id", parent_id);
                    level1_json["children"].append(temp);
                    temp.clear();
                }
            }
            for (auto ps_id : vec)
            {
                if (ps_id == "")
                    continue;
                int pid = std::stoi(ps_id);
                auto row = rows[pid];
                // 处理2级权限
                if (strncasecmp(row[indexOf("ps_level")], "2", 1) == 0)
                {
                    temp["id"] = pid;
                    temp["authName"] = row[indexOf("ps_name")];
                    temp["path"] = row[indexOf("ps_api_path")];
                    auto parent_id = row[indexOf("ps_pid")];
                    temp["pid"] = parent_id;

                    auto &parent_json = findChildrenJsonByMember(json_value["children"], "id", std::to_string(child_par_ids[std::stoi(parent_id)]))["children"];
                    auto &level2_json = findChildrenJsonByMember(parent_json, "id", parent_id);
                    level2_json["children"].append(temp);
                    temp.clear();
                }
            }
        }

        meta["msg"] = "登录成功";
        meta["status"] = 200;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "获取目录列表失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void Roles::giveRole(char *id, char *input_data)
{
    // 创建 JSON 对象
    Json::Value root;
    Json::Reader reader;
    Json::StreamWriterBuilder writer;
    std::string json_string(input_data);

    if (!reader.parse(json_string, root))
    {
        LOG_INFO("sorry, json reader failed");
    }

    std::string sql_string("UPDATE sp_role SET ");
    sql_string += " ps_ids = '" + root["rids"].asString() + "'";
    sql_string += " WHERE role_id = '" + std::string(id) + "';";

    Json::Value ret_root;
    Json::Value data;
    Json::Value meta;
    int mg_id = -1;
    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    LOG_INFO("sql_string=>%s", sql_string.c_str());
    int ret = mysql_query(mysql_, sql_string.c_str());

    if (!ret)
    {
        meta["msg"] = "更新成功";
        meta["status"] = 200;
        ret_root["data"] = "";
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(500, "更新失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void Roles::addRoles(char *input_data)
{
    // 创建 JSON 对象
    Json::Value root;
    Json::Reader reader;
    Json::StreamWriterBuilder writer;

    std::string json_string(input_data);

    if (!reader.parse(json_string, root))
    {
        LOG_INFO("sorry, json reader failed");
    }

    std::string sql_string("INSERT INTO sp_role (role_name, role_desc)");
    sql_string += " VALUES ('" + root["roleName"].asString();
    sql_string += "','" + root["roleDesc"].asString() + "');";

    Json::Value ret_root;
    Json::Value data;
    Json::Value meta;
    int mg_id = -1;
    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    LOG_INFO("sql_string=>%s", sql_string.c_str());
    int ret = mysql_query(mysql_, sql_string.c_str());
    // m_lock.unlock();
    // LOG_DEBUG("ret=>%d", ret);
    if (!ret)
    {
        std::string id = findByKey("sp_role", "role_id", "role_name", root["roleName"].asString());
        root["roleId"] = id;
        root["roleName"] = root["roleName"];
        root["roleDesc"] = root["roleDesc"];
        meta["msg"] = "用户创建成功";
        meta["status"] = 201;
        ret_root["data"] = root;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "用户创建失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void Roles::getRoleById(char *id)
{
    LOG_DEBUG("id=>%s", id);
    clearTableKey();
    getTableKey("sp_role");

    std::string sql_string("SELECT * FROM sp_role WHERE role_id = '" + std::string(id) + "';");
    Json::Value ret_root;
    Json::Value data;
    Json::Value meta;
    int mg_id = -1;
    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");
    int ret = mysql_query(mysql_, sql_string.c_str());
    // LOG_DEBUG("ret=>%d", ret);
    if (!ret)
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        MYSQL_ROW row = mysql_fetch_row(result);
        data["id"] = id;
        data["roleName"] = row[indexOf("role_name")];
        data["roleDesc"] = row[indexOf("role_desc")];

        meta["msg"] = "查询成功";
        meta["status"] = 200;
        ret_root["data"] = data;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "用户查询失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}