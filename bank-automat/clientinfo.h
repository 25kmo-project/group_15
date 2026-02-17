#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>

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

private slots:
    void on_btnBackToMenu_clicked();
};

#endif // CLIENTINFO_H
