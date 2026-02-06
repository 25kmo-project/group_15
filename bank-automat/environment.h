#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>
#include <QList>


class Environment
{
public:
    Environment();
    QString static base_url();
    static QString token;
    static int cardId;
    static int accountId;
    static QList<int> accountIds;

};

#endif // ENVIRONMENT_H
