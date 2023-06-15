#pragma once

#include "logic.h"

class Score : public Logic
{
public:
    Score(MYSQL *mysql, int close_log, int *len, std::string token) : Logic(mysql, close_log, len, token)
    {
    }
    void getScore(char *input_data);
    void addScore(char *input_data);
    void getScoreById(char *id);
    void putScoreById(char *id, char *input_data);
    void deleteScoreById(char *id);

private:
};