#include "login.h"

// 登录
void Login::login(char *input_data)
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

    std::string sql_string("SELECT * FROM sp_manager WHERE mg_name = '");
    sql_string += root["username"].asString();
    sql_string += "' AND mg_pwd = '";
    sql_string += root["password"].asString() + "';";

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
    if (!ret) // 查询成功
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        if (result == NULL || mysql_num_rows(result) == 0)
        {
            errorLogic(404, "用户名或密码错误");
            return;
        }
        while (MYSQL_ROW row = mysql_fetch_row(result))
        {

            mg_id = std::stoi(row[0]);
            LOG_INFO("row=>%d", mg_id);
        }

        data["username"] = root["username"].asString();
        data["token"] = getToken(mg_id);
        meta["msg"] = "登录成功";
        meta["status"] = 200;
        ret_root["data"] = data;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "登录失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}