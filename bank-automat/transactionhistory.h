#ifndef TRANSACTIONHISTORY_H
#define TRANSACTIONHISTORY_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QNetworkAccessManager> 
#include <QNetworkReply>        
#include <QJsonArray>

namespace Ui {
class TransactionHistory;
}

class TransactionHistory : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionHistory(int accId, int cId, QString t, QWidget *parent = nullptr);
    ~TransactionHistory();
    void initTableStyle();
    void displayData();


private:
    Ui::TransactionHistory *ui;
    int currentPage;

    QNetworkAccessManager *manager; 
    QString token;
    int accountId;
    int cardId;

private slots:
    void on_btnBackToMenu_clicked();
    void on_btnNextPage_clicked();
    void on_btnPrevPage_clicked();
};

#endif // TRANSACTIONHISTORY_H
