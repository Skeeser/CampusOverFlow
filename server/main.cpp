#include "config.h"

int main(int argc, char *argv[])
{
    // 数据库信息,登录名,密码,库名
    string user = "root";
    string passwd = "441427";
    string databasename = "serversql";

    Config config;
    config.parse_arg(argc, argv);

    WebServer server;
    server.init(config.PORT, user, passwd, databasename, config.LOGWrite,
                config.OPT_LINGER, config.TRIGMode, config.sql_num, config.thread_num,
                config.close_log, config.actor_model);

    server.log_write();
    server.sql_pool();
    server.thread_pool();
    server.trig_mode();
    server.eventListen();
    server.eventLoop();

    return 0;
}