#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class ClientInfo;
}

class ClientInfo : public QDialog
{
    Q_OBJECT

public:
    explicit ClientInfo(QWidget *parent = nullptr);
    ~ClientInfo();

    void setInfo(const QByteArray &newInfo);

private:
    Ui::ClientInfo *ui;
    QByteArray info;
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;
    int userId;

private slots:
    void on_btnBackToMenu_clicked();
    void on_btnSave_clicked();
    void onUpdateReceived();
};

#endif // CLIENTINFO_H
