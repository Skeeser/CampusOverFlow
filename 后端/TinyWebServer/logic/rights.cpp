#include "rights.h"

void Rights::getRightsLogic(char *input_data)
{
    LOG_DEBUG("input_data=>%s", input_data);
    if (strncasecmp(input_data, "list", 4) == 0)
    {
        rightList();
    }
    else if (strncasecmp(input_data, "tree", 4) == 0)
    {
        rightTree();
    }
}

void Rights::rightList()
{
    std::string sql_string("SELECT * FROM sp_permission_api as api LEFT JOIN sp_permission as main ON main.ps_id = api.ps_id WHERE main.ps_id is not null;");
    Json::Value ret_root;
    Json::Value data;
    Json::Value temp;
    Json::Value meta;

    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

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
            temp["id"] = row[indexOf("ps_id")];
            temp["authName"] = row[indexOf("ps_name")];
            temp["level"] = row[indexOf("ps_level")];
            temp["path"] = row[indexOf("ps_api_path")];
            temp["pid"] = row[indexOf("ps_pid")];
            data.append(temp);
            temp.clear();
        }

        ret_root["data"] = data;
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

void Rights::rightTree()
{

    Json::Value ret_root;
    Json::Value data;
    Json::Value temp;
    Json::Value meta;

    std::string sql_string("SELECT * FROM sp_permission_api as api LEFT JOIN sp_permission as main ON main.ps_id = api.ps_id WHERE main.ps_id is not null;");

    int mg_id = -1;
    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    // std::shared_ptr<std::map<int, Json::Value>> level1_data = std::make_shared<std::map<int, Json::Value>>();
    std::vector<MYSQL_ROW> rows;
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
            rows.push_back(row);
        }

        // LOG_DEBUG("getAllRoles%d", id);
        for (auto row : rows)
        {
            // 处理0级权限
            if (strncasecmp(row[indexOf("ps_level")], "0", 1) == 0)
            {
                int pid = std::stoi(row[indexOf("ps_id")]);
                temp["id"] = pid;
                temp["authName"] = row[indexOf("ps_name")];
                temp["level"] = "0";
                temp["path"] = row[indexOf("ps_api_path")];
                temp["pid"] = row[indexOf("ps_pid")];
                child_par_ids[pid] = std::stoi(row[indexOf("ps_pid")]);
                // temp["children"] = {}; // debug: 要用[]?
                ret_root["data"].append(temp);
                temp.clear();
            }
        }

        for (auto row : rows)
        {
            // 处理1级权限
            if (strncasecmp(row[indexOf("ps_level")], "1", 1) == 0)
            {
                int pid = std::stoi(row[indexOf("ps_id")]);
                temp["id"] = pid;
                temp["authName"] = row[indexOf("ps_name")];
                temp["level"] = "1";
                temp["path"] = row[indexOf("ps_api_path")];
                auto parent_id = row[indexOf("ps_pid")];
                temp["pid"] = parent_id;
                child_par_ids[pid] = std::stoi(parent_id);
                auto &level1_json = findChildrenJsonByMember(ret_root["data"], "id", parent_id);
                level1_json["children"].append(temp);
                temp.clear();
            }
        }
        for (auto row : rows)
        {
            // 处理2级权限
            if (strncasecmp(row[indexOf("ps_level")], "2", 1) == 0)
            {
                int pid = std::stoi(row[indexOf("ps_id")]);
                temp["id"] = pid;
                temp["authName"] = row[indexOf("ps_name")];
                temp["level"] = "2";
                temp["path"] = row[indexOf("ps_api_path")];
                auto parent_id = row[indexOf("ps_pid")];
                temp["pid"] = std::string(parent_id) + "," + std::to_string(child_par_ids[std::stoi(parent_id)]);

                auto &parent_json = findChildrenJsonByMember(ret_root["data"], "id", std::to_string(child_par_ids[std::stoi(parent_id)]))["children"];
                auto &level2_json = findChildrenJsonByMember(parent_json, "id", parent_id);
                level2_json["children"].append(temp);
                temp.clear();
            }
        }

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