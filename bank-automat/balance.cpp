#include "balance.h"
#include "ui_balance.h"

#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

Balance::Balance(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Balance)
{
    ui->setupUi(this);

    networkManager = new QNetworkAccessManager(this);
    getBalance();
}

Balance::~Balance()
{
    delete ui;
}

void Balance::getBalance()
{
    QString path = "transaction/balance";
    QUrl url(Environment::base_url() + path);
    QUrlQuery query;
    query.addQueryItem("account_id", QString::number(Environment::accountId));
    query.addQueryItem("card_id", QString::number(Environment::cardId));
    url.setQuery(query);

    QNetworkRequest request(url);
    QByteArray authHeader = "Bearer " + Environment::token.toUtf8();
    request.setRawHeader("Authorization", authHeader);

    reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &Balance::onBalanceReceived);
}

void Balance::onBalanceReceived()
{

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        double balance = jsonObj["balance"].toDouble();
        ui->labelBalance->setText(QString::number(balance, 'f', 2) + " €");
    } else {
        qDebug() << "Error fetching balance:" << reply->errorString();
        ui->labelBalance->setText("Error!");
    }

    reply->deleteLater();
}

void Balance::on_btnBack_clicked()
{
    this->close();
}
