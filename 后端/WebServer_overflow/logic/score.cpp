#include "score.h"
#include "../util/utils.hpp"

// 根据学号获取成绩
void Score::getScore(char *input_data)
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
        // 判断是否为空
        if (query == "")
        {
            errorLogic(404, "未输入学号");
            return;
        }
        // 要先解码
        query = Utils::urlDecode(query);
        // 参数 grade stuid
        sort_prop = param_hash["sortprop"];
        if (sort_prop == "score")
            sort_prop = "sco.sco_num";
        //  'asc' : 'desc'
        sort_order = param_hash["sortorder"];
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

    std::string sql_string = "SELECT * FROM sp_manager as mgr";
    sql_string += " LEFT JOIN sp_score as sco ON mgr.mg_id = sco.mg_id";
    sql_string += " LEFT JOIN sp_course as curs ON sco.curs_id = curs.curs_id";
    sql_string += " LEFT JOIN sp_class as class ON mgr.class_id = class.class_id";
    sql_string += " WHERE mg_isstu = 1";
    sql_string += " AND mg_stuid LIKE '%" + query + "%'";
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
    getTableKey("sp_score");
    getTableKey("sp_course");
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
            // temp["id"] = row[indexOf("class_id")];
            if (row[indexOf("sco_num")] != NULL && row[indexOf("sco_ispass")] != NULL && row[indexOf("curs_name")] != NULL)
            {
                temp["id"] = row[indexOf("sco_id")];
                temp["score"] = row[indexOf("sco_num")];
                temp["ispass"] = row[indexOf("sco_ispass")];
                temp["course"] = row[indexOf("curs_name")];
            }
            else
            {
                errorLogic(404, "获取成绩的值为空");
                return;
            }
            temp["name"] = row[indexOf("mg_name")];
            temp["grade"] = row[indexOf("class_grade")];
            temp["classname"] = row[indexOf("class_name")];
            temp["college"] = row[indexOf("mg_college")];
            data["score"].append(temp);
            temp.clear();
        }

        meta["msg"] = "查询成绩列表成功";
        meta["status"] = 200;
        root["data"] = data;
        root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "获取成绩列表失败");
        return;
    }

    cpyJson2Buff(&root);
}

// 增加成绩
void Score::addScore(char *input_data)
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
    auto mg_id = findByKey("sp_manager", "mg_id", "mg_stuid", root["stuid"].asString());
    int ispass = 0;

    if (std::stoi(root["scorenum"].asString()) >= 60)
        ispass = 1;

    std::string sql_string("INSERT INTO sp_score (curs_id, mg_id, sco_ispass, sco_num)");
    sql_string += " VALUES ('" + root["courseid"].asString();
    sql_string += "','" + mg_id;
    sql_string += "','" + std::to_string(ispass);
    sql_string += "','" + root["scorenum"].asString() + "');";

    Json::Value ret_root;
    Json::Value data;
    Json::Value meta;
    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    LOG_INFO("sql_string=>%s", sql_string.c_str());
    int ret = mysql_query(mysql_, sql_string.c_str());
    // m_lock.unlock();
    // LOG_DEBUG("ret=>%d", ret);
    if (!ret)
    {
        root["id"] = mg_id;
        meta["msg"] = "成绩添加成功";
        meta["status"] = 201;
        ret_root["data"] = root;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "成绩添加失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

// 获取成绩信息
void Score::getScoreById(char *id)
{

    std::string sql_string = "SELECT * FROM sp_score as sco";
    sql_string += " LEFT JOIN sp_manager as mgr ON sco.mg_id = mgr.mg_id";
    sql_string += " LEFT JOIN sp_course as curs ON scp.curs_id = curs.curs_id";
    sql_string += " WHERE sco_id = '" + std::string(id) + "'";

    clearTableKey();
    getTableKey("sp_score");
    getTableKey("sp_manager");
    getTableKey("sp_course");

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
        data["name"] = row[indexOf("mg_name")];
        data["course"] = row[indexOf("curs_name")];
        data["score"] = row[indexOf("sco_num")];

        meta["msg"] = "查询成功";
        meta["status"] = 200;
        ret_root["data"] = data;
        ret_root["meta"] = meta;
    }
    else
    {
        errorLogic(404, "成绩查询失败");
        return;
    }

    cpyJson2Buff(&ret_root);
}

// 修改成绩信息
void Score::putScoreById(char *id, char *input_data)
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

    int ispass = 0;
    if (std::stoi(root["scorenum"].asString()) >= 60)
        ispass = 1;

    std::string sql_string("UPDATE sp_score SET ");
    sql_string += "sco_ispass = " + std::to_string(ispass) + ", ";
    sql_string += " sco_num = " + root["score"].asString();
    sql_string += " WHERE sco_id = " + std::string(id) + ";";

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

// 删除成绩
void Score::deleteScoreById(char *id)
{

    Json::Value ret_root;
    Json::Value meta;
    std::string sql_string("DELETE FROM sp_score WHERE sco_id = '" + std::string(id) + "';");

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