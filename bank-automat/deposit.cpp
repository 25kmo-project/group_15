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

    setWindowTitle("Deposit");

    // restart timer on open
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    auto *validator = new QDoubleValidator(0.0, 1000000000.0, 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    validator->setLocale(QLocale(QLocale::Finnish, QLocale::Finland));
    ui->lineAmount->setValidator(validator);

    connect(ui->btnConfirm, &QPushButton::clicked, this, &Deposit::onConfirmClicked);
}

void Deposit::onConfirmClicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

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
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        qDebug() << "Deposit response:" << jsonDoc; // ← добавь эту строку
        double newBalance = jsonDoc.object()["new_balance"].toDouble();
        ui->lblInfo->setText(QString("Deposit successful!\nNew balance: %1 €").arg(newBalance, 0, 'f', 2));
    } else {
        ui->lblInfo->setText("Deposit failed.");
        qDebug() << "Deposit error:" << reply->errorString() << data;
    }

    reply->deleteLater();
    reply = nullptr;


}
Deposit::~Deposit()
{
    //cancel any pending network request before destroying
    //problem with app crashing
    if (reply) {
        reply->abort();
        reply->deleteLater();
        reply = nullptr;
    }
    delete ui;
}

void Deposit::on_btnBack_clicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

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
