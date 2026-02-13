#ifndef DEPOSIT_H
#define DEPOSIT_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class Deposit;
}

class Deposit : public QDialog
{
    Q_OBJECT

public:
    explicit Deposit(QWidget *parent = nullptr);
    ~Deposit();

private slots:
    void onConfirmClicked();
    void onReplyFinished();
    void on_btnBack_clicked();

private:
    Ui::Deposit *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;
};

#endif // DEPOSIT_H
