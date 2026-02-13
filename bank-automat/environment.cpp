#include "environment.h"

QString Environment::token = "";
int Environment::cardId = 0;
int Environment::accountId = 0;
QList<int> Environment::accountIds = {};
int Environment::userId = 0;

Environment::Environment() {}

QString Environment::base_url()
{
    return "http://localhost:3000/";
}
