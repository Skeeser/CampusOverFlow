#pragma once
#include "logic.h"

class Rights : public Logic
{
public:
  Rights(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
  {
  }

  // 权限管理
  void getRightsLogic(char *input_data);

private:
};