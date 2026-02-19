#include "withdraw.h"
#include "ui_withdraw.h"
#include "environment.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDoubleValidator>
#include <QDebug>

Withdraw::Withdraw(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Withdraw),
    networkManager(new QNetworkAccessManager(this)),
    reply(nullptr)
{
    ui->setupUi(this);

    // Validator allows only positive decimal values
    auto *validator = new QDoubleValidator(0, 1000000000, 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineAmount->setValidator(validator);

    // Functionality for quick-selection buttons
    connect(ui->withdraw20, &QPushButton::clicked, this, [this]() { performWithdraw(20); });
    connect(ui->withdraw40, &QPushButton::clicked, this, [this]() { performWithdraw(40); });
    connect(ui->withdraw50, &QPushButton::clicked, this, [this]() { performWithdraw(50); });
    connect(ui->withdraw100, &QPushButton::clicked, this, [this]() { performWithdraw(100); });

    // Own amount in the field Enter
    connect(ui->lineAmount, &QLineEdit::returnPressed, this, &Withdraw::onAmountEntered);
}

Withdraw::~Withdraw()
{
    delete ui;
}

// User enters an own amount and presses Enter
void Withdraw::onAmountEntered()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    bool ok = false;
    double amount = ui->lineAmount->text().trimmed().toDouble(&ok);

    if (!ok || amount <= 0.0) {
        ui->lblInfo->setText("Enter a positive amount.");
        return;
    }

    performWithdraw(amount);
}

// Send the withdrawal request
void Withdraw::performWithdraw(double amount)
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    QUrl url(Environment::base_url() + "transaction/withdraw");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + Environment::token.toUtf8());

    QJsonObject body;
    body["account_id"] = Environment::accountId;
    body["card_id"] = Environment::cardId;
    body["amount"] = amount;

    ui->lblInfo->setText("Processing...");
    ui->lineAmount->setEnabled(false);
    for (auto btn : {ui->withdraw20, ui->withdraw40, ui->withdraw50, ui->withdraw100})
        btn->setEnabled(false);

    reply = networkManager->post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, &Withdraw::onReplyFinished);
}

// Backend response
void Withdraw::onReplyFinished()
{
    QByteArray data = reply->readAll();

    // UI back in use
    ui->lineAmount->setEnabled(true);
    for (auto btn : {ui->withdraw20, ui->withdraw40, ui->withdraw50, ui->withdraw100})
        btn->setEnabled(true);

    if (reply->error() == QNetworkReply::NoError) {
        ui->lblInfo->setText("Withdrawal successful.");
        ui->lineAmount->clear();
        qDebug() << "Withdraw OK:" << data;
        reply->deleteLater();
        reply = nullptr;
        getBalance();
    } else {
        ui->lblInfo->setText("Withdrawal failed: " + reply->errorString());
        qDebug() << "Withdraw error:" << reply->errorString() << data;
        reply->deleteLater();
    }

}

void Withdraw::getBalance()
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
    connect(reply, &QNetworkReply::finished, this, &Withdraw::onBalanceReceived);
}

void Withdraw::onBalanceReceived()
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();
        double balance = jsonObj["balance"].toDouble();
        ui->lblInfo->setText("Withdrawal successful. Balance: " +
                             QString::number(balance, 'f', 2) + " €");
    } else {
        ui->lblInfo->setText("Withdrawal successful.");
    }
    reply->deleteLater();
}

void Withdraw::on_btnBack_clicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }
    this->close();
}
