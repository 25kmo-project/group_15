#ifndef TRANSFER_H
#define TRANSFER_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDoubleValidator>

namespace Ui {
class Transfer;
}

class Transfer : public QDialog
{
    Q_OBJECT

public:

    explicit Transfer(int accId, int cId, QString t, QDialog *parent = nullptr);
    ~Transfer();

private slots:
    void on_btnConfirm_clicked();
    void on_btnBack_clicked();

private:
    Ui::Transfer *ui;
    QNetworkAccessManager *manager;

    // Session Data
    int senderAccountId;
    int cardId;
    QString token;

    void setupUiLogic();
};

#endif // TRANSFER_H
