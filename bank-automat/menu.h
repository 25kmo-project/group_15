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

private slots:
    void on_btnBalance_clicked();
    void onBalanceReceived();
    void on_btnDeposit_clicked();
    void on_btnTransactionHistory_clicked();


};


#endif // MENU_H
