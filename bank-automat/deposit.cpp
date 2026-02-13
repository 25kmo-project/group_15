#include "deposit.h"
#include "ui_deposit.h"
#include "environment.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDoubleValidator>
#include <QDebug>
#include <QTimer>


Deposit::Deposit(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Deposit),
    networkManager(new QNetworkAccessManager(this)),
    reply(nullptr)

{
    ui->setupUi(this);

    auto *validator = new QDoubleValidator(0, 1000000000, 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineAmount->setValidator(validator);

    connect(ui->btnConfirm, &QPushButton::clicked, this, &Deposit::onConfirmClicked);
}

Deposit::~Deposit()
{
    delete ui;
}
void Deposit::onConfirmClicked()
{
    bool ok = false;
    double amount = ui->lineAmount->text().trimmed().toDouble(&ok);

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

    // Palautetaan käyttöliittymä käyttöön (paitsi onnistumisessa suljetaan kohta)
    ui->btnConfirm->setEnabled(true);
    ui->lineAmount->setEnabled(true);

    if (reply->error() == QNetworkReply::NoError) {
        ui->lblInfo->setText("Deposit successful.");
        ui->lineAmount->clear();
        qDebug() << "Deposit OK:" << data;

        // Estetään lisäklikkailu ja annetaan viestin näkyä hetki
        ui->btnConfirm->setEnabled(false);
        ui->lineAmount->setEnabled(false);

        // Sulje 2 sekunnin jälkeen -> palaa Menuun (exec() loppuu)
        QTimer::singleShot(2000, this, [this]() {
            this->accept();
        });

    } else {
        ui->lblInfo->setText("Deposit failed.");
        qDebug() << "Deposit error:" << reply->errorString() << data;
    }

    reply->deleteLater();
}
