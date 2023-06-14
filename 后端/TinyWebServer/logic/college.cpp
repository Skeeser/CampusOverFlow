#include "college.h"

// 获取学院列表
void College::getColleges()
{

    // 执行sql语句
    Json::Value root;
    Json::Value temp;
    Json::Value meta;

    std::string sql_string = "SELECT * FROM sp_college";

    if (mysql_ == NULL)
    {
        LOG_INFO("mysql is NULL!");
        return;
    }
    // 在获取前先清除
    clearTableKey();
    getTableKey("sp_college");

    int ret = mysql_query(mysql_, sql_string.c_str());
    if (!ret) // 查询成功de
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        while (MYSQL_ROW row = mysql_fetch_row(result))
        {
            temp["id"] = row[indexOf("cge_id")];
            temp["collegename"] = row[indexOf("cge_name")];
            root["data"].append(temp);
            temp.clear();
        }

        meta["msg"] = "查询学院列表成功";
        meta["status"] = 200;
        root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "获取学院列表失败");
        return;
    }

    cpyJson2Buff(&root);
}