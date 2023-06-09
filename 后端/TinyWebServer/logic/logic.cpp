#include "logic.h"

#define WRITE_BUFFER_SIZE 4096
#define SECRET_KEY "3MnJb57tW9TAvkYFQEDUgLdSRuBzmXcZ"

// 获取索引值
std::string Logic::getToken(int mg_id)
{
    return jwt::create()
        .set_issuer("auth0")
        .set_type("JWS")
        .set_payload_claim("mg_id", jwt::claim(std::to_string(mg_id)))
        .sign(jwt::algorithm::hs256{SECRET_KEY});
}

// 验证token并且解析其中的用户id
bool Logic::checkToken(std::string token, int &mg_id)
{
    auto decoded = jwt::decode(token);
    mg_id = -1;
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{SECRET_KEY})
                        .with_issuer("auth0");
    try
    {
        verifier.verify(decoded);
        for (auto i : decoded.get_payload_json())
        {
            if (i.first == "mg_id")
            {
                mg_id = stoi(i.second.to_str());
            }
        }
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_INFO("tokken error for %s", e.what());
        return false;
    }
}

// sql查询
std::string Logic::findByKey(std::string table_name, std::string ret_name, std::string col_name, std::string key)
{
    std::string sql_string("SELECT " + ret_name + " FROM " + table_name);
    sql_string += " WHERE " + col_name + " = '" + key + "';";
    int ret_count = -1;
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");
    // LOG_DEBUG("str=>%s", sql_string.c_str());
    int ret = mysql_query(mysql_, sql_string.c_str());

    if (!ret) // 查询成功
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        MYSQL_ROW row = mysql_fetch_row(result);
        return row[0];
    }
    // LOG_DEBUG("ret_count=>%d", ret_count);
    return "";
}

// 登录
void Logic::loginLogic(char *user_data)
{

    // 创建 JSON 对象
    Json::Value root;

    Json::Reader reader;
    Json::StreamWriterBuilder writer;
    std::string json_string(user_data);
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

// 菜单
void Logic::menuLogic()
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

// 模糊查询用户数量
int Logic::getUsersCountByKey(std::string table_name, std::string col_name, std::string key)
{
    std::string sql_string("SELECT count(*) as count FROM " + table_name);
    sql_string += " WHERE " + col_name + " LIKE '%" + key + "%' ;";
    int ret_count = -1;
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");
    // LOG_DEBUG("str=>%s", sql_string.c_str());
    int ret = mysql_query(mysql_, sql_string.c_str());

    if (!ret) // 查询成功de
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);
        MYSQL_ROW row = mysql_fetch_row(result);
        ret_count = std::stoi(row[0]);
    }
    // LOG_DEBUG("ret_count=>%d", ret_count);
    return ret_count;
}

// 解析Get 请求数据。 todo 用了哈希表，怎么优化
std::shared_ptr<std::unordered_map<std::string, std::string>> Logic::parseGetData(char *input_data)
{
    std::shared_ptr<std::unordered_map<std::string, std::string>> param_hash = std::make_shared<std::unordered_map<std::string, std::string>>();

    // LOG_DEBUG("%s", input_data);
    int limit = 0;
    // 解析查询参数
    std::string prase_str = std::string(input_data) + "&";
    std::string temp_str = "";
    std::string key = "";
    std::string value = "";

    while (prase_str != "")
    {
        auto temp_pos = prase_str.find_first_of("&");
        if (temp_pos == std::string::npos)
            break;
        temp_str = prase_str.substr(0, temp_pos);
        prase_str.erase(0, temp_pos + 1);
        limit++;
        // 防止死循环
        if (limit >= 500)
            break;
        auto kv_pos = temp_str.find("=");
        if (kv_pos == std::string::npos)
            break;
        key = temp_str.substr(0, kv_pos);
        value = temp_str.substr(kv_pos + 1, temp_str.size());

        if (key != "")
            (*param_hash)[key] = value;
        else
        {
            errorLogic(400, "参数错误");
            return nullptr;
        }
    }
    return param_hash;
}

// 用户管理
void Logic::getUsersLogic(char *input_data)
{
    int page_num = -1;
    int page_size = -1;
    std::string is_stu = "-1";
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
        is_stu = param_hash["isstu"];
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

void Logic::addUserLogic(char *input_data)
{
    // 创建 JSON 对象
    Json::Value root;
    Json::Reader reader;
    Json::StreamWriterBuilder writer;
    std::string json_string(input_data);
    std::string role_id = "-1";
    std::string class_id = "-1";
    // 获取当前时间的时间戳
    std::time_t currentTime = std::time(nullptr);
    // 使用字符串流构建时间字符串
    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");

    if (!reader.parse(json_string, root))
    {
        LOG_INFO("sorry, json reader failed");
    }
    if (root["isstu"].asString() == "1")
        role_id = findByKey("sp_role", "role_id", "role_name", "学生");
    else if (root["isstu"].asString() == "0")
        role_id = findByKey("sp_role", "role_id", "role_name", "老师");

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

    std::string sql_string("INSERT INTO sp_manager (mg_name, mg_pwd, mg_mobile, mg_email, mg_time, role_id, mg_college, mg_stuid, class_id, mg_isstu)");
    sql_string += " VALUES ('" + root["username"].asString();
    sql_string += "','" + root["password"].asString();
    sql_string += "','" + root["mobile"].asString();
    sql_string += "','" + root["email"].asString();
    sql_string += "','" + ss.str();
    sql_string += "','" + role_id;
    sql_string += "','" + root["college"].asString();
    sql_string += "','" + root["stuid"].asString();
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
void Logic::getUserByIdLogic(char *id)
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
void Logic::putUserByIdLogic(char *id, char *input_data)
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
void Logic::deleteUserByIdLogic(char *id)
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

void Logic::getRightsLogic(char *input_data)
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

// 获取表的所有键的名字
void Logic::getTableKey(string table_name)
{

    std::string sql_string("SHOW COLUMNS from ");
    sql_string += table_name + " ;";

    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    int ret = mysql_query(mysql_, sql_string.c_str());

    // LOG_DEBUG("ret=>%d", ret);
    if (!ret) // 查询成功de
    {
        // 从表中检索完整的结果集
        MYSQL_RES *result = mysql_store_result(mysql_);

        while (MYSQL_ROW row = mysql_fetch_row(result))
        {

            key_vector_->push_back(row[0]);
            // LOG_INFO("row=>%d", mg_id);
        }
    }
    else
    {
        return;
    }
}

// 清除存储键名的容器
void Logic::clearTableKey()
{
    key_vector_->clear();
}

// 错误处理
void Logic::errorLogic(int status, std::string msg)
{
    Json::Value ret_root;
    Json::Value data;
    Json::Value meta;

    data["userid"] = user_id_;
    meta["msg"] = msg;
    meta["status"] = status;
    ret_root["data"] = data;
    ret_root["meta"] = meta;

    cpyJson2Buff(&ret_root);
}

// 将结果的json对象写入到暂存数组中
void Logic::cpyJson2Buff(Json::Value *ret_root)
{
    Json::StreamWriterBuilder writer;

    // 清空
    memset(temp_buff_, '\0', WRITE_BUFFER_SIZE);

    // 将 JSON 对象转换为字符串
    std::string jsonString = Json::writeString(writer, *ret_root);

    *len_ = jsonString.size();
    // LOG_DEBUG("json_string = %s, len = %d", jsonString.c_str(), *len_);
    if (*len_ <= WRITE_BUFFER_SIZE)
    {
        strncpy(temp_buff_, jsonString.c_str(), *len_);
        // LOG_DEBUG("ret_json=>%s", temp_buff_);
    }
}

// 获取键名对应的索引值, 用之前要先调用getTableKey()
int Logic::indexOf(string key_name)
{
    for (int i = 0; i < key_vector_->size(); i++)
    {
        if (key_vector_->at(i) == key_name)
        {
            return i;
        }
    }
    // 默认返回第一个
    return 0;
}