#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>


class Environment
{
public:
    Environment();
    QString static base_url();
    static QString token;
    static int cardId;
    static int accountId;

};

#endif // ENVIRONMENT_H
