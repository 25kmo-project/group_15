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
#include <QTimer>

#include "environment.h"
#include "deposit.h"
#include "transfer.h"
#include "currency.h"
#include "balance.h"
#include "mainwindow.h"


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
    //transaction
    void on_btnTransactionHistory_clicked();
    //deposit
    void on_btnDeposit_clicked();
    //withdraw
    void on_btnWithdrawal_clicked(); // huolehdi että nimi vastaa .ui:n painiketta
    //transfer
    void on_btnTransfer_clicked();
    //my profile
    void on_btnMyProfile_clicked();
    void onMyProfileReceived();
    //log out
    void on_btnLogOut_clicked();
    //currency
    void on_btnCurrency_clicked();
    //automatic log out
    void autoLogOut();
    void closeAllNotMain(MainWindow *mainWindow);
};


#endif // MENU_H
