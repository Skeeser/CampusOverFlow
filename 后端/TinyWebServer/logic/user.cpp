#include "user.h"

// 用户管理
void User::getUsers(char *input_data)
{
    int page_num = -1;
    int page_size = -1;
    std::string is_stu = "-1";
    std::string query = "";
    // 排序的参数
    std::string sort_prop = "";
    // 排序的顺序
    std::string sort_order = "";

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
        is_stu = param_hash["isstu"];
        // 参数 grade stuid
        sort_prop = param_hash["sortprop"];
        if (sort_prop == "grade")
            sort_prop = "class.class_grade";
        else if (sort_prop == "stuid")
            sort_prop = "mgr.mg_stuid";
        //  'asc' : 'desc'
        sort_order = param_hash["sortorder"];
        // todo 实现排序
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
    sql_string += " AND mg_name LIKE '%" + query + "%'";
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

void User::addUser(char *input_data)
{
    // 创建 JSON 对象
    Json::Value root;
    Json::Reader reader;
    Json::StreamWriterBuilder writer;
    std::string json_string(input_data);
    std::string role_id = "-1";
    std::string class_id = "-1";
    std::string stu_id = "-1";
    // 获取当前时间的时间戳
    std::time_t currentTime = std::time(nullptr);
    // 使用字符串流构建时间字符串
    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");

    if (!reader.parse(json_string, root))
    {
        LOG_INFO("sorry, json reader failed");
    }

    // 先插入班级表中
    std::string sql_insert_class("INSERT INTO sp_class (class_name, class_grade) ");
    sql_insert_class += " SELECT '" + root["class"].asString();
    sql_insert_class += "','" + root["grade"].asString() + "'";
    sql_insert_class += " WHERE NOT EXISTS ( SELECT 1 FROM sp_class WHERE class_name = '" + root["class"].asString() + "' AND class_grade = '" + root["grade"].asString() + "');";
    int ret1 = mysql_query(mysql_, sql_insert_class.c_str());
    int ret2 = mysql_query(mysql_, "SELECT LAST_INSERT_ID();");
    if (!ret1 && !ret2)
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        MYSQL_ROW row = mysql_fetch_row(result);
        class_id = row[0];
    }
    LOG_DEBUG("insert class_id:%s", class_id.c_str());
    LOG_INFO("sql_string=>%s", sql_insert_class.c_str());

    stu_id = root["stuid"].asString();
    // 插入学生表中
    if (root["isstu"].asString() == "1")
        role_id = findByKey("sp_role", "role_id", "role_name", "学生");
    else if (root["isstu"].asString() == "0")
    {
        role_id = findByKey("sp_role", "role_id", "role_name", "老师");
        stu_id = "0";
        class_id = "3";
    }

    std::string sql_string("INSERT INTO sp_manager (mg_name, mg_pwd, mg_mobile, mg_email, mg_time, role_id, mg_college, mg_stuid, class_id, mg_isstu)");
    sql_string += " VALUES ('" + root["username"].asString();
    sql_string += "','" + root["password"].asString();
    sql_string += "','" + root["mobile"].asString();
    sql_string += "','" + root["email"].asString();
    sql_string += "','" + ss.str();
    sql_string += "','" + role_id;
    sql_string += "','" + root["college"].asString();
    sql_string += "','" + stu_id;
    sql_string += "','" + class_id;
    sql_string += "','" + root["isstu"].asString() + "');";

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
        std::string id = findByKey("sp_manager", "mg_id", "mg_name", root["username"].asString());
        root["id"] = id;
        root["role_id"] = role_id;
        root["create_time"] = ss.str();
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

// 通过id获取用户信息
void User::getUserById(char *id)
{
    LOG_DEBUG("id=>%s", id);
    getTableKey("sp_manager");

    std::string sql_string("SELECT * FROM sp_manager WHERE mg_id = '" + std::string(id) + "';");
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
        data["username"] = row[indexOf("mg_name")];
        data["role_id"] = row[indexOf("role_id")];
        data["mobile"] = row[indexOf("mg_mobile")];
        data["email"] = row[indexOf("mg_email")];
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

// 修改用户信息
void User::putUserById(char *id, char *input_data)
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

    std::string role_id = findByKey("sp_manager", "role_id", "mg_id", id);

    std::string sql_string("UPDATE sp_manager SET ");
    sql_string += " mg_mobile = '" + root["mobile"].asString() + "', ";
    sql_string += " mg_email = '" + root["email"].asString() + "'  ";
    sql_string += " WHERE mg_id = '" + std::string(id) + "';";

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
        root["role_id"] = role_id;
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

// 根据id删除该用户
void User::deleteUserById(char *id)
{
    Json::Value ret_root;
    Json::Value meta;
    std::string sql_string("DELETE FROM sp_manager WHERE mg_id = '" + std::string(id) + "';");

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

void User::putUserRole(char *id, char *rid)
{
    // 创建 JSON 对象
    Json::Value root;
    Json::StreamWriterBuilder writer;

    std::string sql_string("UPDATE sp_manager SET ");
    sql_string += " role_id = '" + std::string(rid) + "'";
    sql_string += " WHERE mg_id = '" + std::string(id) + "';";

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
        root["role_id"] = rid;
        // root["mobile"] = root["mobile"];
        // root["email"] = root["email"];

        meta["msg"] = "设置角色成功";
        meta["status"] = 200;
        ret_root["data"] = root;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(500, "设置角色失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}