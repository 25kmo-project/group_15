#ifndef WITHDRAW_H
#define WITHDRAW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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
    void onConfirmClicked();
    void onAmountEntered();    // user push the own amount - button
    void onReplyFinished();    // Backendin response
    void on_btnBack_clicked();
    void onBalanceReceived();

private:
    Ui::Withdraw *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;

    bool isValidAmount(int amount);
};

#endif // WITHDRAW_H




