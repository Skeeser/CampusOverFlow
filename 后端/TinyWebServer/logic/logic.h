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

class Logic
{
public:
    // todo debug len可能会出问题
    Logic(MYSQL *mysql, int close_log, int *len) : mysql_(mysql),
                                                   m_close_log(close_log),
                                                   len_(len)
    {
    }

    Logic(MYSQL *mysql, int close_log, int *len, std::string token) : mysql_(mysql),
                                                                      m_close_log(close_log),
                                                                      len_(len)
    {
        LOG_DEBUG("get_token=>%s", token.c_str());
        is_token_vaild_ = checkToken(token, user_id_);
        // is_token_vaild_ = true;
        // user_id_ = 500;

        key_vector_ = std::make_shared<std::vector<std::string>>();
    }
    ~Logic()
    {
        delete[] temp_buff_;
    };
    std::string getToken(int mg_id);

    char *getData()
    {
        return temp_buff_;
    }

protected:
    MYSQL *mysql_;
    int m_close_log;
    int user_id_;
    bool is_token_vaild_;
    char *temp_buff_;
    int *len_;

    // 常见sql操作
    std::string findByKey(std::string table_name, std::string ret_name, std::string col_name, std::string key);
    void getTableResult(std::string table_name, MYSQL_RES *result);
    bool checkToken(std::string token, int &mg_id);
    std::shared_ptr<std::vector<std::string>> key_vector_;
    void getTableKey(string table_name);
    void clearTableKey();
    void errorLogic(int status, std::string msg);
    void cpyJson2Buff(Json::Value *ret_root);
    int indexOf(string key_name);
    std::shared_ptr<std::unordered_map<std::string, std::string>> parseGetData(char *input_data);
    int getUsersCountByKey(std::string table_name, std::string col_name, std::string key);

    // Json操作
    Json::Value &findChildrenJsonByMember(Json::Value &parent, std::string key, std::string value);
};