#include "clientinfo.h"
#include "ui_clientinfo.h"


#include <QDebug>
#include <QJsonObject>


ClientInfo::ClientInfo(QWidget *parent): QDialog(parent), ui(new Ui::ClientInfo)
{
    ui->setupUi(this);

    setWindowTitle("Information");

}

ClientInfo::~ClientInfo()
{
    delete ui;
}

void ClientInfo::setInfo(const QByteArray &newInfo)
{

    QJsonDocument jsonDoc = QJsonDocument::fromJson(newInfo);

    if (jsonDoc.isNull()) {
        qDebug() << "error:invalid json";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    ui->labelFname->setText(jsonObject["user_name"].toString());
    ui->labelLname->setText(jsonObject["user_lastname"].toString());
    ui->labelAddress->setText(jsonObject["user_address"].toString());
    ui->labelEmail->setText(jsonObject["user_email"].toString());
    ui->labelPhonenumber->setText(jsonObject["user_phonenumber"].toString());
}

void ClientInfo::on_btnBackToMenu_clicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    this->close();
}
