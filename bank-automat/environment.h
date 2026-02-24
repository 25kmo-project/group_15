#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>
#include <QList>
#include <QTimer>
#include <QDateTime>

class Environment
{
public:
    Environment();
    QString static base_url();
    //JWT token for authentication
    static QString token;
    //Currently selected card id
    static int cardId;
    //Currently selected account id
    static int accountId;
    //List of account ids for the user
    static QList<int> accountIds;
    //Current user ID
    static int userId;
    //Card:debit or credit
    static QString cardType;

    //Timer for automatic logout
    static QTimer *timerLogOut;

    //Tracks if balance screen was viewed
    static bool viewedBalance;
    //Tracks if profile screen was viewed
    static bool viewedProfile;
    //Tracks if currency screen was viewed
    static bool viewedCurrency;
    //Tracks if transaction history was viewed
    static bool viewedTransactionHistory;



    //Timestamp of last balance view
    static QDateTime timeViewedBalance;
    //Timestamp of last profile view
    static QDateTime timeViewedProfile;
    //Timestamp of last currency view
    static QDateTime timeViewedCurrency;
    //Timestamp of last transaction history view
    static QDateTime timeViewedTransactionHistory;

};

#endif // ENVIRONMENT_H
