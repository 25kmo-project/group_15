#include "receipt.h"
#include "ui_receipt.h"
#include "environment.h"

#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDebug>

Receipt::Receipt(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Receipt),
    networkManager(new QNetworkAccessManager(this)),
    reply(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Kuitti");

    // Käynnistä haku heti kun dialogi avataan
    fetchReceipt();
}

Receipt::~Receipt()
{
    if (reply) {
        reply->abort();
        reply->deleteLater();
        reply = nullptr;
    }
    delete ui;
}

void Receipt::fetchReceipt()
{
    if (Environment::token.isEmpty()) {
        showError("Token puuttuu. Kirjaudu uudelleen.");
        return;
    }

    if (Environment::cardId == 0 || Environment::accountId == 0) {
        showError("cardId tai accountId puuttuu. Valitse tili tai kirjaudu uudelleen.");
        return;
    }

    QUrl url(Environment::base_url() + "transaction/receipt");
    QUrlQuery query;
    query.addQueryItem("account_id", QString::number(Environment::accountId));
    query.addQueryItem("card_id", QString::number(Environment::cardId));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", ("Bearer " + Environment::token).toUtf8());

    reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &Receipt::onReceiptReceived);

    ui->txtReceipt->setPlainText("Haetaan kuittia...");
}

void Receipt::onReceiptReceived()
{
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        showError("Kuittia ei voitu hakea: " + reply->errorString());
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();
    reply = nullptr;

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (!doc.isObject()) {
        showError("Virheellinen vastaus backendiltä (JSON).");
        return;
    }

    QJsonObject obj = doc.object();
    ui->txtReceipt->setPlainText(buildReceiptText(obj));
}

QString Receipt::buildReceiptText(const QJsonObject &obj) const
{
    QString app = obj.value("app").toString("Pankkiautomaattisovellus");
    QString timestampIso = obj.value("timestamp").toString();
    double balance = obj.value("balance").toDouble(0.0);

    QDateTime dt = QDateTime::fromString(timestampIso, Qt::ISODate);
    QString dtStr = dt.isValid()
                        ? dt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                        : QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");

    QString text;
    text += "Tulosta kuitti\n\n";
    text += app + "\n\n";
    text += "Pvm ja aika: " + dtStr + "\n";
    text += "Saldo: " + QString::number(balance, 'f', 2) + " €\n\n";

    QJsonArray events = obj.value("events").toArray();
    if (events.isEmpty()) {
        text += "Ei tapahtumia tämän session aikana.\n";
        return text;
    }

    text += "Tapahtumat:\n";
    for (const QJsonValue &v : events) {
        QJsonObject e = v.toObject();
        QString type = e.value("transaction_type").toString();

        // amount tulee backendiltä stringinä "100.00"
        QString amountStr = e.value("amount").toString();
        amountStr.replace(",", ".");
        double amount = amountStr.toDouble();

        // transaction_date tulee muodossa "yyyy-MM-dd HH:mm:ss"
        QString tRaw = e.value("transaction_date").toString();
        QDateTime tdt = QDateTime::fromString(tRaw, "yyyy-MM-dd HH:mm:ss");

        QString tStr = tdt.isValid()
                           ? tdt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                           : tRaw;

        text += "- " + tStr + "  " + type + "  " + QString::number(amount, 'f', 2) + " €\n";

    }

    return text;
}

void Receipt::on_btnBack_clicked()
{
    accept();
}

void Receipt::showError(const QString &msg)
{
    ui->txtReceipt->setPlainText("Virhe:\n" + msg);
}
