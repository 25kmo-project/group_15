#ifndef BALANCE_H
#define BALANCE_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "environment.h"

namespace Ui {
class Balance;
}

class Balance : public QDialog
{
    Q_OBJECT

public:
    explicit Balance(QWidget *parent = nullptr);
    ~Balance();

private:
    Ui::Balance *ui;

    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;

    void getBalance();

private slots:
    void onBalanceReceived();
    void on_btnBack_clicked();

};

#endif // BALANCE_H
