#ifndef RECEIPT_H
#define RECEIPT_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class Receipt; }
QT_END_NAMESPACE

class Receipt : public QDialog
{
    Q_OBJECT

public:
    explicit Receipt(QWidget *parent = nullptr);
    ~Receipt();

private slots:
    void fetchReceipt();
    void onReceiptReceived();
    void on_btnBack_clicked();

private:
    void showError(const QString &msg);
    QString buildReceiptText(const QJsonObject &obj) const;

    Ui::Receipt *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;
};

#endif // RECEIPT_H
