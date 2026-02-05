#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>


class Environment
{
public:
    Environment();
    QString static base_url();
    static QString token;
    static int card_id;
    static int account_id;

};

#endif // ENVIRONMENT_H
