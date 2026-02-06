#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //login
    connect(ui->btnLogin, &QPushButton::clicked, this, &MainWindow::btnLoginSlot);


    manager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//login
void MainWindow::btnLoginSlot()
{
    QString url = Environment::base_url()+"login";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jObject;
    jObject.insert("card_id", ui->textCardId->text());
    jObject.insert("pin_code", ui->textPincode->text());
    QJsonDocument jsonDoc(jObject);
    reply = manager->post(request, jsonDoc.toJson());
    connect(reply, &QNetworkReply::finished, this, &MainWindow::loginAction);
}
//login
void MainWindow::loginAction()
{
    QByteArray responseData=reply->readAll();
    reply->deleteLater();

    //jos ei ole yhteyttä backendiin
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if(!jsonDoc.isObject()) {
        ui->labelInfo->setText("Sorry, we have some technical issues");
        //jos tarvitaan qt konsoliin viestin
        //qDebug() << "Problems with backend" << responseData;
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    //kun backend lähettää viesti, näytetään
    if(jsonObject.contains("message")) {
        QString message = jsonObject["message"].toString();
        ui->labelInfo->setText(message);
        //jos tarvitaan qt konsoliin viestin
        //qDebug() << "Server message:" << message;
    }

    //server lähettää token ja card_id qt konsoliin
    //DebitvsCredit ikkunan asetus
    if(jsonObject.contains("token")) {
        Environment:: token = jsonObject["token"].toString();
        Environment:: cardId = jsonObject["card_id"].toInt();
        Environment::accountIds.clear();
        QJsonArray accounts = jsonObject["account_id"].toArray();
        for (const QJsonValue &value : accounts) {
            Environment::accountIds.append(value.toInt());
        }
        qDebug() << "Total accounts found:" << Environment::accountIds.size();

        DebitvsCredit *objDebitvsCredit = new DebitvsCredit(this);
        objDebitvsCredit->setToken(Environment::token);
        objDebitvsCredit->show();

        qDebug() << "login ok";
        qDebug() << "cardid:" << Environment::cardId;
        qDebug() << "accountid:" << Environment::accountIds;
        qDebug() << "token:" << Environment::token;
    }



}
