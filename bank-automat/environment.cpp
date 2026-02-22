#include "environment.h"

#include <QDateTime>

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

QTimer* Environment::timerLogOut = nullptr;

bool Environment::viewedBalance = false;
bool Environment::viewedProfile = false;
bool Environment::viewedCurrency = false;
bool Environment::viewedTransactionHistory = false;
QDateTime Environment::timeViewedBalance;
QDateTime Environment::timeViewedProfile;
QDateTime Environment::timeViewedCurrency;
QDateTime Environment::timeViewedTransactionHistory;
