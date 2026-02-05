#include "environment.h"

QString Environment::token = "";
int Environment::cardId = 0;
int Environment::accountId = 0;

Environment::Environment() {}

QString Environment::base_url()
{
    return "http://localhost:3000/";
}
