#include "environment.h"

QString Environment::token = "";
int Environment::card_id = 0;
int Environment::account_id = 0;

Environment::Environment() {}

QString Environment::base_url()
{
    return "http://localhost:3000";
}
