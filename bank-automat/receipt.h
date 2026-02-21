#ifndef RECEIPT_H
#define RECEIPT_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

namespace Ui {
class Receipt;
}

class Receipt : public QDialog
{
    Q_OBJECT

public:
    explicit Receipt(QWidget *parent = nullptr);
    ~Receipt();

private slots:
    void on_btnBack_clicked();
    void onReceiptReceived();

private:
    Ui::Receipt *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;

    bool triedFull;

    void fetchReceipt();
    void showError(const QString &msg);

    QString buildReceiptHtml(const QJsonObject &obj) const;
};

#endif // RECEIPT_H
