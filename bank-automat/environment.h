#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>
#include <QList>
#include <QTimer>

class Environment
{
public:
    Environment();
    QString static base_url();
    static QString token;
    static int cardId;
    static int accountId;
    static QList<int> accountIds;
    static int userId;
    static QTimer *timerLogOut;

};

#endif // ENVIRONMENT_H
