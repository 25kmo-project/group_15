#include "withdraw.h"
#include "ui_withdraw.h"
#include "environment.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDoubleValidator>
#include <QDebug>
#include <QTimer>

Withdraw::Withdraw(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Withdraw),
    networkManager(new QNetworkAccessManager(this)),
    reply(nullptr)
{
    ui->setupUi(this);

    // Validator sallii vain positiiviset desimaalit
    auto *validator = new QDoubleValidator(0, 1000000000, 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineAmount->setValidator(validator);

    // Pika-nappien toiminta
    connect(ui->withdraw20, &QPushButton::clicked, this, [this]() { performWithdraw(20); });
    connect(ui->withdraw40, &QPushButton::clicked, this, [this]() { performWithdraw(40); });
    connect(ui->withdraw50, &QPushButton::clicked, this, [this]() { performWithdraw(50); });
    connect(ui->withdraw100, &QPushButton::clicked, this, [this]() { performWithdraw(100); });

    // Oma summa syötekentän Enter
    connect(ui->lineAmount, &QLineEdit::returnPressed, this, &Withdraw::onAmountEntered);
}

Withdraw::~Withdraw()
{
    delete ui;
}

// Käyttäjä syöttää oman summan ja painaa Enter
void Withdraw::onAmountEntered()
{
    bool ok = false;
    double amount = ui->lineAmount->text().trimmed().toDouble(&ok);

    if (!ok || amount <= 0.0) {
        ui->lblInfo->setText("Enter a positive amount.");
        return;
    }

    performWithdraw(amount);
}

// Lähettää withdraw requestin
void Withdraw::performWithdraw(double amount)
{
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

// Backendin vastaus
void Withdraw::onReplyFinished()
{
    QByteArray data = reply->readAll();

    // UI takaisin käyttöön
    ui->lineAmount->setEnabled(true);
    for (auto btn : {ui->withdraw20, ui->withdraw40, ui->withdraw50, ui->withdraw100})
        btn->setEnabled(true);

    if (reply->error() == QNetworkReply::NoError) {
        ui->lblInfo->setText("Withdrawal successful.");
        ui->lineAmount->clear();
        QTimer::singleShot(2000, this, [this]() { this->accept(); }); // Sulje dialogi hetken kuluttua
        qDebug() << "Withdraw OK:" << data;
    } else {
        ui->lblInfo->setText("Withdrawal failed: " + reply->errorString());
        qDebug() << "Withdraw error:" << reply->errorString() << data;
    }

    reply->deleteLater();
}
