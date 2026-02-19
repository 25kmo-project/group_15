#ifndef WITHDRAW_H
#define WITHDRAW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

#include "environment.h"

namespace Ui {
class Withdraw;
}

class Withdraw : public QDialog
{
    Q_OBJECT

public:
    explicit Withdraw(QWidget *parent = nullptr);
    ~Withdraw();

private slots:
    void onAmountEntered();    // user push the own amount - button
    void onReplyFinished();    // Backendin response
    void on_btnBack_clicked();
    void onBalanceReceived();

private:
    Ui::Withdraw *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;

    void performWithdraw(double amount); // send request

    void getBalance();
};

#endif // WITHDRAW_H


