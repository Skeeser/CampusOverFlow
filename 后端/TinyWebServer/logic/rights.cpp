#include "rights.h"


void Rights::getRightsLogic(char *input_data)
{
    LOG_DEBUG("input_data=>%s", input_data);
    if (strncasecmp(input_data, "list", 4))
    {
    }
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

void Rights::getRoles();
