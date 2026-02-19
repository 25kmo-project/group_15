#ifndef CURRENCY_H
#define CURRENCY_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui { class Currency; }

class Currency : public QDialog
{
    Q_OBJECT

public:
    explicit Currency(QString token, QWidget *parent = nullptr);
    ~Currency();

private slots:
    void fetchRates();
    void onRatesReceived();
    void on_btnBack_clicked();

private:
    Ui::Currency *ui;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QString m_token;

    void setupRequest(QNetworkRequest &request, const QString &path);
};

#endif // CURRENCY_H
