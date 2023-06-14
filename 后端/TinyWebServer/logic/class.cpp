#include "class.h"

void Class::getClass(char *input_data)
{
    int page_num = -1;
    int page_size = -1;
    std::string query = "";
    // 排序的参数
    std::string sort_prop = "";
    // 排序的顺序
    std::string sort_order = "";
    std::string college_id = "";
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
        // 参数 grade stuid
        sort_prop = param_hash["sortprop"];
        // if (sort_prop == "grade")
        //     sort_prop = "class.class_grade";
        // else if (sort_prop == "stuid")
        //     sort_prop = "mgr.mg_stuid";
        //  'asc' : 'desc'
        sort_order = param_hash["sortorder"];
        college_id = param_hash["collegeid"];
    }
    else
        return;

    // 计算页的范围
    int count = getUsersCountByKey("sp_class", "class_name", query);
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

    std::string sql_string = "SELECT * FROM sp_class as class LEFT JOIN sp_college as col ON class.cge_id = col.cge_id";
    sql_string += " WHERE class_name LIKE '%" + query + "%'";
    if (sort_prop != "" && sort_order != "")
        sql_string += " ORDER BY " + sort_prop + " " + sort_order;
    sql_string += " LIMIT " + std::to_string(offset) + "," + std::to_string(page_size) + ";";
    LOG_DEBUG("SQL:\n%s", sql_string.c_str());
    if (mysql_ == NULL)
    {
        LOG_INFO("mysql is NULL!");
        return;
    }
    // 在获取前先清除
    clearTableKey();
    getTableKey("sp_class");
    getTableKey("sp_college");

    int ret = mysql_query(mysql_, sql_string.c_str());
    if (!ret) // 查询成功de
    {
        data["total"] = count;
        data["pagenum"] = page_num;

        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        while (MYSQL_ROW row = mysql_fetch_row(result))
        {
            temp["id"] = row[indexOf("class_id")];
            temp["classname"] = row[indexOf("class_name")];
            temp["college"] = row[indexOf("cge_name")];
            temp["grade"] = row[indexOf("class_grade")];
            temp["collegeid"] = row[indexOf("cge_id")];
            data["class"].append(temp);
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

// 增加班级
void Class::addClass(char *input_data)
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

    std::string sql_string("INSERT INTO sp_class (class_name, class_grade, cge_id)");
    sql_string += " VALUES ('" + root["classname"].asString();
    sql_string += "','" + root["grade"].asString();
    sql_string += "','" + root["collegeid"].asString() + "');";

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
        std::string id = findByKey("sp_class", "class_id", "class_name", root["classname"].asString());
        root["id"] = id;
        meta["msg"] = "班级创建成功";
        meta["status"] = 201;
        ret_root["data"] = root;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "班级创建失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void Class::getClassById(char *id)
{
    clearTableKey();
    getTableKey("sp_class");

    std::string sql_string("SELECT * FROM sp_class WHERE class_id = '" + std::string(id) + "';");
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
        data["classname"] = row[indexOf("class_name")];
        data["grade"] = row[indexOf("class_grade")];
        data["collegeid"] = row[indexOf("cge_id")];

        meta["msg"] = "查询成功";
        meta["status"] = 200;
        ret_root["data"] = data;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "班级查询失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void Class::putClassById(char *id, char *input_data)
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

    std::string sql_string("UPDATE sp_class SET ");
    sql_string += " class_name = '" + root["classname"].asString() + "', ";
    sql_string += " class_grade = '" + root["grade"].asString() + "'  ";
    sql_string += " cge_id = '" + root["collegeid"].asString() + "'  ";
    sql_string += " WHERE class_id = '" + std::string(id) + "';";

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
        root["id"] = id;
        // root["mobile"] = root["mobile"];
        // root["email"] = root["email"];

        meta["msg"] = "更新成功";
        meta["status"] = 200;
        ret_root["data"] = root;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(500, "更新失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void ClassdeleteClassById(char *id)
{

    Json::Value ret_root;
    Json::Value meta;
    std::string sql_string("DELETE FROM sp_class WHERE class_id = '" + std::string(id) + "';");

    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    int ret = mysql_query(mysql_, sql_string.c_str());
    if (!ret)
    {
        meta["msg"] = "删除成功";
        meta["status"] = 200;
        ret_root["data"] = "";
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(500, "删除失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}