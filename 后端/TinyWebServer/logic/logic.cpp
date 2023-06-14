#include "logic.h"

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

// 获取某个表的所有数据
void Logic::getTableResult(std::string table_name, MYSQL_RES *result)
{
    std::string sql_string("SELECT * FROM " + table_name + " ;");

    // m_lock.lock();
    if (mysql_ == NULL)
        LOG_INFO("mysql is NULL!");

    // 在获取前先清除
    clearTableKey();
    getTableKey(table_name);

    int ret = mysql_query(mysql_, sql_string.c_str());

    if (!ret) // 查询成功
    {
        // 从表中检索完整的结果集
        result = mysql_store_result(mysql_);
    }
    else
    {
        result = nullptr;
        errorLogic(404, "查询失败");
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

    // 将 JSON 对象转换为字符串
    std::string jsonString = Json::writeString(writer, *ret_root);

    *len_ = jsonString.size();
    // LOG_DEBUG("json_string = %, len = %d", *len_); // , jsonString.c_str()

    temp_buff_ = new char[*len_ + 1];
    strncpy(temp_buff_, jsonString.c_str(), *len_);
    temp_buff_[*len_] = '\0';
    // LOG_DEBUG("ret_json=>%s", temp_buff_);
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

// 根据父json中的子成员是否匹配来查找子Json, 若相同则返回第一个匹配的
Json::Value &Logic::findChildrenJsonByMember(Json::Value &parent, std::string key, std::string value)
{

    for (Json::Value &json_value : parent)
    {
        if (json_value[key].asString() == value)
            return json_value;
    }
}