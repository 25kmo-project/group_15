#ifndef MENU_H
#define MENU_H

#include <QDialog>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "environment.h"
#include "deposit.h"
#include "transfer.h"

class TransactionHistory;

namespace Ui {
class Menu;
}

class Menu : public QDialog
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

private:
    Ui::Menu *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;

    void setupRequest(QNetworkRequest &request, const QString &path);
    TransactionHistory* ptrTransactionHistory = nullptr;
    Transfer* ptrTransfer = nullptr;

private slots:
    //balance
    void on_btnBalance_clicked();
    void onBalanceReceived();
    //transaction
    void on_btnTransactionHistory_clicked();
    //deposit
    void on_btnDeposit_clicked();
    //Transfer
    void on_btnTransfer_clicked();
    //my profile
    void on_btnMyProfile_clicked();
    void onMyProfileReceived();


};


#endif // MENU_H
