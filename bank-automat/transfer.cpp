#include "transfer.h"
#include "ui_transfer.h"
#include "environment.h"

Transfer::Transfer(int accId, int cId, QString t, QDialog *parent) :
    QDialog(parent),
    ui(new Ui::Transfer),
    senderAccountId(accId),
    cardId(cId),
    token(t)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    setupUiLogic();
}

Transfer::~Transfer()
{
    delete ui;
}

void Transfer::setupUiLogic()
{
    // Restrict amount to numbers with 2 decimal places
    ui->leAmount->setValidator(new QDoubleValidator(0.0, 1000000.0, 2, this));
    ui->lblErrorDisplay->clear();
}

void Transfer::on_btnConfirm_clicked()
{
    QString receiverAcc = ui->leTargetAccount->text();
    double amount = ui->leAmount->text().toDouble();

    if (receiverAcc.length() < 18) {
        ui->lblErrorDisplay->setText("Account number must be 18 characters.");
        return;
    }
    if (amount <= 0) {
        ui->lblErrorDisplay->setText("Please enter a valid amount.");
        return;
    }

    QJsonObject json;
    json.insert("sender_account_id", senderAccountId);
    json.insert("receiver_account_number", receiverAcc);
    json.insert("card_id", cardId);
    json.insert("amount", amount);

    QUrl url(Environment::base_url() + "transaction/transfer");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());

    QJsonDocument doc(json);
    qDebug() << "Sending JSON:" << doc.toJson(QJsonDocument::Compact); 

    QNetworkReply *reply = manager->post(request, doc.toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            double newBalance = doc.object()["new_balance"].toString().toDouble();

            QMessageBox::information(this, "Success",
                                     QString("Transfer successful!\nNew balance: %1 €").arg(newBalance, 0, 'f', 2));
            this->close();
        } else {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QString errorMsg = doc.object()["message"].toString();

            // Map SQL signals to user-friendly messages
            if (errorMsg.contains("INSUFFICIENT_FUNDS")) errorMsg = "Insufficient funds.";
            else if (errorMsg.contains("RECEIVER_NOT_FOUND")) errorMsg = "Receiver account not found.";
            else if (errorMsg.contains("UNAUTHORIZED")) errorMsg = "Access denied.";
            else errorMsg = "Transfer failed. Please try again.";

            ui->lblErrorDisplay->setText(errorMsg);
        }
        reply->deleteLater();
    });
}

void Transfer::on_btnBack_clicked()
{
    this->close();
}
