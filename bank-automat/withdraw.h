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
    void onAmountEntered();    // Käyttäjä painaa oma summa -näppäintä
    void onReplyFinished();    // Backendin vastaus

private:
    Ui::Withdraw *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;

    void performWithdraw(double amount); // Lähettää requestin
};

#endif // WITHDRAW_H


