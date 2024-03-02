#pragma once
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>
#include <string>
#include <jwt-cpp/jwt.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <memory>
#include "../lock/locker.h"
#include "../log/log.h"

class Token
{
public:
    Token(int close_log) : m_close_log(close_log)
    {
    }

protected:
    int m_close_log;
    std::string getToken(int mg_id);
    bool checkToken(std::string token, int &mg_id);
};

class Logic : public Token
{
public:
    // todo debug len可能会出问题
    Logic(MYSQL *mysql, int close_log, int *len) : Token(close_log),
                                                   mysql_(mysql),
                                                   len_(len)
    {
    }

    Logic(MYSQL *mysql, int close_log, int *len, std::string token) : Token(close_log),
                                                                      mysql_(mysql),
                                                                      len_(len)

    {
        LOG_DEBUG("get_token=>%s", token.c_str());
        is_token_vaild_ = checkToken(token, user_id_);
        key_vector_ = std::make_shared<std::vector<std::string>>();
    }
    ~Logic()
    {
        delete[] temp_buff_;
    };

    char *getData()
    {
        return temp_buff_;
    }

protected:
    MYSQL *mysql_;
    int user_id_;
    bool is_token_vaild_;
    char *temp_buff_;
    int *len_;
    // 存储表的所有键
    std::shared_ptr<std::vector<std::string>> key_vector_;

    // 常见sql操作
    std::string findByKey(std::string table_name, std::string ret_name, std::string col_name, std::string key);
    void getTableResult(std::string table_name, MYSQL_RES *result);
    void getTableKey(string table_name);
    void clearTableKey();
    int indexOf(string key_name);
    int getUsersCountByKey(std::string table_name, std::string col_name, std::string key);

    void errorLogic(int status, std::string msg);
    void cpyJson2Buff(Json::Value *ret_root);
    std::shared_ptr<std::unordered_map<std::string, std::string>> parseGetData(char *input_data);

    // Json操作
    Json::Value &findChildrenJsonByMember(Json::Value &parent, std::string key, std::string value);
};