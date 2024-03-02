#include "menus.h"

// 菜单
void Menus::getMenus(char *input_data)
{
    // 先验证token
    if (!is_token_vaild_)
    {
        errorLogic(403, "token验证不通过");
        return;
    }

    std::string sql_string("SELECT * FROM sp_permission_api as api LEFT JOIN sp_permission as main ON main.ps_id = api.ps_id WHERE main.ps_id is not null;");

    Json::Value ret_root;
    Json::Value data;
    Json::Value temp;
    Json::Value meta;

    int level0_num = 0;
    int mg_id = -1;
    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    std::shared_ptr<std::map<int, Json::Value>> level1_data = std::make_shared<std::map<int, Json::Value>>();

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
            temp["path"] = row[indexOf("ps_api_path")];
            // temp["children"] = {}; // debug: 要用[]?
            int pid = stoi(row[indexOf("ps_pid")]);
            // temp["order"] = permission.ps_api_order
            // 根据 level判断
            // 一级菜单
            if (strncasecmp(row[indexOf("ps_level")], "0", 1) == 0)
            {
                level0_num++;
                ret_root["data"]
                    .append(temp);
            }
            // 二级菜单
            else if (strncasecmp(row[indexOf("ps_level")], "1", 1) == 0)
            {
                if (pid)
                    (*level1_data)[pid].append(temp);
            }
            temp.clear();
        }

        // 遍历ret_root中data

        for (Json::Value &json_value : ret_root["data"])
        {
            json_value["children"] = (*level1_data)[stoi(json_value["id"].asString().c_str())];
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