#include "clientinfo.h"
#include "ui_clientinfo.h"
#include "environment.h"
#include <QMessageBox>
#include <QNetworkRequest>
#include <QDebug>


ClientInfo::ClientInfo(QWidget *parent): QDialog(parent), ui(new Ui::ClientInfo)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);
    userId = 0;
}

ClientInfo::~ClientInfo()
{
    delete ui;
}

void ClientInfo::setInfo(const QByteArray &newInfo)
{
    info = newInfo;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(info);

    if (jsonDoc.isNull()) {
        qDebug() << "error:invalid json";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    userId=jsonObject["user_id"].toInt();

    ui->labelFname->setText(jsonObject["user_name"].toString());
    ui->labelLname->setText(jsonObject["user_lastname"].toString());
    ui->lineAddress->setText(jsonObject["user_address"].toString());
    ui->lineEmail->setText(jsonObject["user_email"].toString());
    ui->linePhonenumber->setText(jsonObject["user_phonenumber"].toString());
}

void ClientInfo::on_btnBackToMenu_clicked()
{
    this->close();
}

void ClientInfo::on_btnSave_clicked()
{
    //delete empty spaces at the begining and at the end of the line
    QString email = ui->lineEmail->text().trimmed();
    QString phone = ui->linePhonenumber->text().trimmed();
    QString address = ui->lineAddress->text().trimmed();

    //all fields shoud be filled
    if (email.isEmpty() || phone.isEmpty() || address.isEmpty()) {
        ui->labelMessage->setText("All fields must be filled");
        return;
    }

    //email should have @ and .
    if (!email.contains("@") || !email.contains(".")) {
        ui->labelMessage->setText("Error, invalid email format");
        return;
    }

    QJsonObject updateData;
    updateData["user_address"] = address;
    updateData["user_email"] = email;
    updateData["user_phonenumber"] = phone;

    QJsonDocument jsonDoc(updateData);

    //put
    QString path = "user/" + QString::number(userId);
    QUrl url(Environment::base_url() + path);

    QNetworkRequest request(url);
    QByteArray authHeader = "Bearer " + Environment::token.toUtf8();
    request.setRawHeader("Authorization", authHeader);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "Updating user profile:" << url.toString();
    qDebug() << "Data:" << jsonDoc.toJson();

    reply = networkManager->put(request, jsonDoc.toJson());
    connect(reply, &QNetworkReply::finished, this, &ClientInfo::onUpdateReceived);
}

void ClientInfo::onUpdateReceived()
{
    if (reply->error() == QNetworkReply::NoError) {
        ui->labelMessage->setText("Profile updated successfully");
        qDebug() << "Client's profile updated successfully";
    } else {
        ui->labelMessage->setText("Failed to update: " + reply->errorString());
        qDebug() << "Error updating client's profile:" << reply->errorString();
    }

    reply->deleteLater();
}

