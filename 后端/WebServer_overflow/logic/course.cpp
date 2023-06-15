#include "course.h"
#include "../util/utils.hpp"
// 课程管理
void Course::getCourse(char *input_data)
{
    int page_num = -1;
    int page_size = -1;
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
        // 要先解码
        query = Utils::urlDecode(query);
        // 参数 grade stuid
        sort_prop = param_hash["sortprop"];
        if (sort_prop == "coursenum")
            sort_prop = "curs.curs_num";
        //  'asc' : 'desc'
        sort_order = param_hash["sortorder"];
        // todo 实现排序
    }
    else
        return;

    // 计算页的范围
    int count = getUsersCountByKey("sp_course", "curs_name", query);
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

    std::string sql_string = "SELECT * FROM sp_course as curs LEFT JOIN sp_college as col ON curs.cge_id = col.cge_id";
    sql_string += " WHERE curs_name LIKE '%" + query + "%'";
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
    getTableKey("sp_course");
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
            temp["id"] = row[indexOf("curs_id")];
            temp["coursename"] = row[indexOf("curs_name")];
            temp["coursenum"] = row[indexOf("curs_num")];
            temp["collegeid"] = row[indexOf("cge_id")];
            temp["college"] = row[indexOf("cge_name")];
            data["courses"].append(temp);
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

// 添加课程
void Course::addCourse(char *input_data)
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

    std::string sql_string("INSERT INTO sp_course (curs_name, curs_num, cge_id)");
    sql_string += " VALUES ('" + root["coursename"].asString();
    sql_string += "','" + root["coursenum"].asString();
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
        std::string id = findByKey("sp_course", "curs_id", "curs_name", root["coursename"].asString());
        root["id"] = id;
        meta["msg"] = "课程创建成功";
        meta["status"] = 201;
        ret_root["data"] = root;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "课程创建失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void Course::getCourseById(char *id)
{
    clearTableKey();
    getTableKey("sp_course");

    std::string sql_string("SELECT * FROM sp_course WHERE curs_id = '" + std::string(id) + "';");
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
        data["coursename"] = row[indexOf("curs_name")];
        data["coursenum"] = row[indexOf("curs_num")];
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

void Course::getCourseByStuid(char *id)
{

    std::string sql_string = "SELECT * FROM sp_manager as mgr";
    sql_string += " LEFT JOIN sp_college as col ON mgr.mg_college = col.cge_name";
    sql_string += " LEFT JOIN sp_course as curs ON col.cge_id = curs.cge_id";
    sql_string += " WHERE mg_isstu = 1";
    sql_string += " AND mg_stuid = '" + std::string(id) + "'";

    Json::Value ret_root;
    Json::Value temp;
    Json::Value meta;

    clearTableKey();
    getTableKey("sp_manager");
    getTableKey("sp_college");
    getTableKey("sp_course");

    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");
    int ret = mysql_query(mysql_, sql_string.c_str());
    // LOG_DEBUG("ret=>%d", ret);
    if (!ret)
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        while (MYSQL_ROW row = mysql_fetch_row(result))
        {
            temp["id"] = row[indexOf("curs_id")];
            temp["coursename"] = row[indexOf("curs_name")];
            ret_root["data"].append(temp);
            temp.clear();
        }

        meta["msg"] = "查询课程列表成功";
        meta["status"] = 200;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "课程查询失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void Course::getCourseByCollegeid(char *id)
{
    std::string sql_string = "SELECT * FROM sp_course as curs";
    sql_string += " LEFT JOIN sp_college as col ON col.cge_id = curs.cge_id";
    sql_string += " WHERE curs.cge_id = '" + std::string(id) + "'";

    Json::Value ret_root;
    Json::Value temp;
    Json::Value meta;

    clearTableKey();
    getTableKey("sp_course");
    getTableKey("sp_college");

    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");
    int ret = mysql_query(mysql_, sql_string.c_str());
    // LOG_DEBUG("ret=>%d", ret);
    if (!ret)
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        while (MYSQL_ROW row = mysql_fetch_row(result))
        {
            temp["id"] = row[indexOf("curs_id")];
            temp["collegename"] = row[indexOf("curs_name")];
            ret_root["data"].append(temp);
            temp.clear();
        }

        meta["msg"] = "查询课程列表成功";
        meta["status"] = 200;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "课程查询失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

void Course::putCourseById(char *id, char *input_data)
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

    std::string sql_string("UPDATE sp_course SET ");
    sql_string += " curs_name = '" + root["coursename"].asString() + "',";
    sql_string += " curs_num = " + root["coursenum"].asString() + ",";
    sql_string += " cge_id = " + root["collegeid"].asString();
    sql_string += " WHERE curs_id = " + std::string(id) + ";";

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

// 根据id删除课程
void Course::deleteCourseById(char *id)
{

    Json::Value ret_root;
    Json::Value meta;
    std::string sql_string("DELETE FROM sp_course WHERE curs_id = '" + std::string(id) + "';");

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