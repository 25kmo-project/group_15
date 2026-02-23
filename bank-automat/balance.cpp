#include "balance.h"
#include "ui_balance.h"

#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

Balance::Balance(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Balance)
    , reply(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("Balance");
    ui->detailContainer->hide();

    networkManager = new QNetworkAccessManager(this);
    getBalance();

    Environment::viewedBalance = true;
    Environment::timeViewedBalance = QDateTime::currentDateTime();

    // restart timer on open
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }
}

Balance::~Balance()
{
    //prevent app crashing
    if (reply) {
        reply->abort();
        reply->deleteLater();
        reply = nullptr;
    }
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
        qDebug() << "RAW RESPONSE:" << responseData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        double balance = jsonObj["balance"].toDouble();
        double availableFunds = jsonObj["available_funds"].toDouble();
        QString accountType = jsonObj["account_type"].toString();
        double creditLimit = jsonObj["credit_limit"].toDouble();
        ui->labelBalance->setText(QString::number(availableFunds, 'f', 2) + " €");
        if (accountType == "CREDIT") {
            ui->detailContainer->show(); 
            ui->labelLimit->setText("Credit Limit: " + QString::number(creditLimit, 'f', 2) + " €");
            ui->labelDebt->setText("Current Debt: " + QString::number(balance, 'f', 2) + " €");
        } else {
            if (ui->detailContainer) {
                ui->detailContainer->hide();
            }
        }
    } else {
        qDebug() << "Error fetching balance:" << reply->errorString();
        ui->labelBalance->setText("Error!");
    }

    reply->deleteLater();
    reply=nullptr;
}

void Balance::on_btnBack_clicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    this->close();
}
