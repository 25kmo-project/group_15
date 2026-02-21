#include "withdraw.h"
#include "ui_withdraw.h"
#include "environment.h"  // contains token, accountId, cardId
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QTimer>
#include <QDebug>
#include <QIntValidator>

Withdraw::Withdraw(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Withdraw),
    networkManager(new QNetworkAccessManager(this)),
    reply(nullptr)
{
    ui->setupUi(this);

    // Only allow integers 20-10000
    ui->lineAmount->setValidator(new QIntValidator(20, 10000, this));

    // Connect quick withdrawal buttons
    connect(ui->withdraw20, &QPushButton::clicked, this, &Withdraw::onConfirmClicked);
    connect(ui->withdraw40, &QPushButton::clicked, this, &Withdraw::onConfirmClicked);
    connect(ui->withdraw50, &QPushButton::clicked, this, &Withdraw::onConfirmClicked);
    connect(ui->withdraw100, &QPushButton::clicked, this, &Withdraw::onConfirmClicked);

    // Enter key for custom amount
    connect(ui->lineAmount, &QLineEdit::returnPressed, this, &Withdraw::onConfirmClicked);
}

Withdraw::~Withdraw()
{
    delete ui;
}

// Frontend validation: minimum 20 and only 20/50 combinations
bool Withdraw::isValidAmount(int amount)
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }
    if (amount < 20) return false;

    for (int fifties = 0; fifties * 50 <= amount; fifties++) {
        if ((amount - fifties * 50) % 20 == 0)
            return true;
    }
    return false;
}

void Withdraw::onConfirmClicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    int amount = 0;
    QPushButton *button = qobject_cast<QPushButton*>(sender());

    if (button == ui->withdraw20) amount = 20;
    else if (button == ui->withdraw40) amount = 40;
    else if (button == ui->withdraw50) amount = 50;
    else if (button == ui->withdraw100) amount = 100;
    else amount = ui->lineAmount->text().toInt();

    if (!isValidAmount(amount)) {
        ui->lblInfo->setText("Invalid amount: must be at least 20 and a combination of 20/50€ bills.");
        return;
    }

    ui->lblInfo->setText("Processing...");
    ui->lineAmount->setEnabled(false);
    ui->withdraw20->setEnabled(false);
    ui->withdraw40->setEnabled(false);
    ui->withdraw50->setEnabled(false);
    ui->withdraw100->setEnabled(false);

    QUrl url(Environment::base_url() + "transaction/withdraw");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QByteArray authHeader = "Bearer " + Environment::token.toUtf8();
    request.setRawHeader("Authorization", authHeader);

    QJsonObject json;
    json["amount"] = amount;
    json["account_id"] = Environment::accountId;
    json["card_id"] = Environment::cardId;

    reply = networkManager->post(request, QJsonDocument(json).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, &Withdraw::onReplyFinished);
}

void Withdraw::onReplyFinished()
{
    QByteArray data = reply->readAll();

    // Re-enable UI
    ui->lineAmount->setEnabled(true);
    ui->withdraw20->setEnabled(true);
    ui->withdraw40->setEnabled(true);
    ui->withdraw50->setEnabled(true);
    ui->withdraw100->setEnabled(true);

    // Default message
    QString message;

    // Get HTTP status code
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (status >= 200 && status < 300) {
        // Successful withdrawal
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();
        message = jsonObj["message"].toString();
        if (message.isEmpty()) message = "Withdrawal successful";

        ui->lblInfo->setText(message);

        // Close dialog automatically after 3 seconds
        QTimer::singleShot(3000, this, &QDialog::accept);

    } else {
        // Failed withdrawal (invalid amount, insufficient funds, etc.)
        ui->lblInfo->setText("Withdrawal failed");  // always generic
        qDebug() << "Withdrawal failed, status:" << status << "data:" << data;
    }

    reply->deleteLater();
    reply = nullptr;
}

void Withdraw::on_btnBack_clicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }
    this->close();
}
