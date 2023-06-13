#include "class.h"

void Class::getClass(char *input_data)
{
    int page_num = -1;
    int page_size = -1;
    std::string college_id = "-1";
    std::string query = "";
    // 获取请求的数据
    auto ret_hash_ptr = parseGetData(input_data);
    if (ret_hash_ptr != nullptr)
    {
        auto param_hash = *ret_hash_ptr;
        // 验证参数
        if (param_hash["pagenum"] != "" && param_hash["pagesize"] != "")
        {
            page_num = std::stoi(param_hash["pagenum"]);
            page_size = std::stoi(param_hash["pagesize"]);
        }
        else
        {
            errorLogic(400, "参数错误");
            return;
        }
        query = param_hash["query"];
        college_id = param_hash["collegeid"];
    }
    else
        return;

    // 计算页的范围
    int count = getUsersCountByKey("sp_manager", "mg_name", query);
    int pageCount = ceil(count / page_size);
    int offset = (page_num - 1) * page_size;
    if (offset >= count)
    {
        offset = count;
    }

    // 执行sql语句
    Json::Value root;
    Json::Value temp;
    Json::Value data;
    Json::Value meta;

    std::string sql_string = "SELECT * FROM sp_manager as mgr LEFT JOIN sp_role as role ON mgr.role_id = role.role_id LEFT JOIN sp_class as class ON mgr.class_id = class.class_id";
    sql_string += " WHERE mg_isstu = " + is_stu;
    sql_string += " AND mg_name LIKE '%" + query + "%' LIMIT " + std::to_string(offset) + "," + std::to_string(page_size) + ";";
    LOG_DEBUG("SQL:\n%s", sql_string.c_str());
    if (mysql_ == NULL)
    {
        LOG_INFO("mysql is NULL!");
        return;
    }
    // 在获取前先清除
    clearTableKey();
    getTableKey("sp_manager");
    getTableKey("sp_role");
    getTableKey("sp_class");

    int ret = mysql_query(mysql_, sql_string.c_str());
    if (!ret) // 查询成功de
    {
        data["total"] = count;
        data["pagenum"] = page_num;

        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        while (MYSQL_ROW row = mysql_fetch_row(result))
        {
            temp["id"] = row[indexOf("mg_id")];
            temp["role_name"] = row[indexOf("role_name")];
            temp["username"] = row[indexOf("mg_name")];
            temp["create_time"] = row[indexOf("mg_time")];
            temp["mobile"] = row[indexOf("mg_mobile")];
            temp["email"] = row[indexOf("mg_email")];
            temp["isstu"] = row[indexOf("mg_isstu")];
            temp["grade"] = row[indexOf("class_grade")];
            temp["college"] = row[indexOf("mg_college")];
            temp["class"] = row[indexOf("class_name")];
            temp["stuid"] = row[indexOf("mg_stuid")];
            data["users"].append(temp);
            temp.clear();
        }

        meta["msg"] = "查询用户列表成功";
        meta["status"] = 200;
        root["data"] = data;
        root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "获取用户列表失败");
        return;
    }

    cpyJson2Buff(&root);
}