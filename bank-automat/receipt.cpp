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
    setWindowTitle("Receipt");

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
        showError("Token missing. login again.");
        return;
    }

    if (Environment::cardId == 0 || Environment::accountId == 0) {
        showError("cardId or accountId missing. select account or login again.");
        return;
    }

    QUrl url(Environment::base_url() + "transaction/receipt/full");
    QUrlQuery query;
    query.addQueryItem("account_id", QString::number(Environment::accountId));
    query.addQueryItem("card_id", QString::number(Environment::cardId));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", ("Bearer " + Environment::token).toUtf8());

    reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &Receipt::onReceiptReceived);

    ui->txtReceipt->setPlainText("Fetching receipt...");
}

void Receipt::onReceiptReceived()
{
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        showError("could not fetch receipt: " + reply->errorString());
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();
    reply = nullptr;

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (!doc.isObject()) {
        showError("Invalid response from the backend (JSON).");
        return;
    }

    QJsonObject obj = doc.object();
    ui->txtReceipt->setPlainText(buildReceiptText(obj));
}

QString Receipt::buildReceiptText(const QJsonObject &obj) const
{
    QString app = "Bank ATM Application";
    QString timestampIso = obj.value("timestamp").toString();
    double balance = obj.value("balance").toDouble(0.0);

    QDateTime dt = QDateTime::fromString(timestampIso, Qt::ISODate);
    QString dtStr = dt.isValid()
                        ? dt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                        : QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");

    QString text;
    text += "Print receipt\n\n";
    text += app + "\n\n";
    text += "Date and time: " + dtStr + "\n";
    text += "Balance: " + QString::number(balance, 'f', 2) + " €\n\n";

    // --- MYPROFILE (profile) ---
    QJsonValue profileVal = obj.value("profile");
    if (profileVal.isObject()) {
        QJsonObject p = profileVal.toObject();
        QString first = p.value("user_name").toString();
        QString last  = p.value("user_lastname").toString();
        QString addr  = p.value("user_address").toString();
        QString email = p.value("user_email").toString();
        QString phone = p.value("user_phonenumber").toString();

        text += "Customer information:\n";
        if (!first.isEmpty() || !last.isEmpty())
            text += "- Name " + first + " " + last + "\n";
        if (!addr.isEmpty())
            text += "- Address: " + addr + "\n";
        if (!email.isEmpty())
            text += "- Email: " + email + "\n";
        if (!phone.isEmpty())
            text += "- Phone number: " + phone + "\n";
        text += "\n";
    }

    // --- CURRENCY (currency.latest + currency.change) ---
    QJsonValue currencyVal = obj.value("currency");
    if (currencyVal.isObject()) {
        QJsonObject c = currencyVal.toObject();

        // latest
        if (c.value("latest").isObject()) {
            QJsonObject latest = c.value("latest").toObject();
            QString base = latest.value("base").toString("EUR");
            QString date = latest.value("date").toString();
            double usd = latest.value("USD").toDouble(0.0);
            double gbp = latest.value("GBP").toDouble(0.0);

            text += "exchange rates (latest):\n";
            text += "- Base: " + base + "\n";
            if (!date.isEmpty()) text += "- date: " + date + "\n";
            if (usd > 0) text += "- USD: " + QString::number(usd, 'f', 6) + "\n";
            if (gbp > 0) text += "- GBP: " + QString::number(gbp, 'f', 6) + "\n";
            text += "\n";
        }

        // change
        if (c.value("change").isObject()) {
            QJsonObject ch = c.value("change").toObject();
            QString base = ch.value("base").toString("EUR");
            QString prevDate = ch.value("prevDate").toString();
            QString lastDate = ch.value("lastDate").toString();

            text += "exchange rate (" + base + "):\n";
            if (!prevDate.isEmpty() && !lastDate.isEmpty())
                text += "- " + prevDate + " -> " + lastDate + "\n";

            auto addSymbol = [&](const QString &sym){
                if (!ch.value(sym).isObject()) return;
                QJsonObject s = ch.value(sym).toObject();
                double rate = s.value("rate").toDouble(0.0);
                double abs  = s.value("abs").toDouble(0.0);
                double pct  = s.value("pct").toDouble(0.0);

                text += "- " + sym + ": " + QString::number(rate, 'f', 6)
                        + " (";
                text += (abs >= 0 ? "+" : "") + QString::number(abs, 'f', 6);
                text += ", ";
                text += (pct >= 0 ? "+" : "") + QString::number(pct, 'f', 2) + "%)\n";
            };

            addSymbol("USD");
            addSymbol("GBP");
            text += "\n";
        }
    }

    // --- EVENTS ---
    QJsonArray events = obj.value("events").toArray();
    if (events.isEmpty()) {
        text += "No events during this session.\n";
        return text;
    }

    text += "Events:\n";
    for (const QJsonValue &v : events) {
        QJsonObject e = v.toObject();
        QString type = e.value("transaction_type").toString();


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
    ui->txtReceipt->setPlainText("Error:\n" + msg);
}
