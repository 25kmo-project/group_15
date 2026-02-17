#include "deposit.h"
#include "ui_deposit.h"
#include "environment.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDoubleValidator>
#include <QDebug>
#include <QLocale>
#include <QUrlQuery>


Deposit::Deposit(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Deposit),
    networkManager(new QNetworkAccessManager(this)),
    reply(nullptr)
{
    ui->setupUi(this);

    auto *validator = new QDoubleValidator(0.0, 1000000000.0, 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    validator->setLocale(QLocale(QLocale::Finnish, QLocale::Finland));
    ui->lineAmount->setValidator(validator);

    connect(ui->btnConfirm, &QPushButton::clicked, this, &Deposit::onConfirmClicked);
}

void Deposit::onConfirmClicked()
{
    QString text = ui->lineAmount->text().trimmed();

    bool ok = false;
    QLocale fi(QLocale::Finnish, QLocale::Finland);
    double amount = fi.toDouble(text, &ok);

    if (!ok) { // accept comma
        amount = QLocale::c().toDouble(text, &ok);
    }

    if (!ok || amount <= 0.0) {
        ui->lblInfo->setText("Enter a positive amount.");
        return;
    }

    QUrl url(Environment::base_url() + "transaction/deposit");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QByteArray authHeader = "Bearer " + Environment::token.toUtf8();
    request.setRawHeader("Authorization", authHeader);

    QJsonObject body;
    body["account_id"] = Environment::accountId;
    body["card_id"] = Environment::cardId;
    body["amount"] = amount;

    ui->btnConfirm->setEnabled(false);
    ui->lblInfo->setText("Processing...");

    reply = networkManager->post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, &Deposit::onReplyFinished);
}

void Deposit::onReplyFinished()
{
    QByteArray data = reply->readAll();

    // Re-enable UI after deposit attempt
    ui->btnConfirm->setEnabled(true);
    ui->lineAmount->setEnabled(true);

    if (reply->error() == QNetworkReply::NoError) {
        ui->lineAmount->clear();
        qDebug() << "Deposit successful:" << data;

        reply->deleteLater();
        reply = nullptr;

        getBalance();

    } else {
        ui->lblInfo->setText("Deposit failed.");
        qDebug() << "Deposit error:" << reply->errorString() << data;
        reply->deleteLater();
    }


}
Deposit::~Deposit()
{
    delete ui;
}

void Deposit::on_btnBack_clicked()
{
    reject();  // close window and return to menu
}

void Deposit::onBalanceReceived()
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();
        double balance = jsonObj["balance"].toDouble();
        ui->lblInfo->setText("Deposit successful. Balance: " +
                             QString::number(balance, 'f', 2) + " €");
    } else {
        ui->lblInfo->setText("Deposit successful.");
    }

    reply->deleteLater();
}

void Deposit::getBalance()
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
    connect(reply, &QNetworkReply::finished, this, &Deposit::onBalanceReceived);
}
