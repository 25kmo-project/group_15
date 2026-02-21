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

    setWindowTitle("Digital Receipt");

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
    //ui->txtReceipt->setPlainText(buildReceiptText(obj));
    ui->txtReceipt->setHtml(buildReceiptText(obj));
}

/*QString Receipt::buildReceiptText(const QJsonObject &obj) const
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


    QString Receipt::buildReceiptText(const QJsonObject &obj) const
    {
        QString app = obj.value("app").toString("Pankkiautomaattisovellus");
        QString timestampIso = obj.value("timestamp").toString();
        double balance = obj.value("balance").toDouble(0.0);

        QDateTime dt = QDateTime::fromString(timestampIso, Qt::ISODate);
        QString dtStr = dt.isValid()
                            ? dt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                            : QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");

        QString html;
        html += "<div style='font-family: Arial; padding: 10px;'>";
        html += "<h2 style='color: #1a73e8; text-align: center;'>" + app + "</h2>";
        html += "<hr style='border: 1px solid #e0e0e0;'>";
        html += "<p style='color: #666; font-size: 13px;'>Date: <b>" + dtStr + "</b></p>";
        html += "<p style='font-size: 16px;'>Balance: <b style='color: #1a73e8;'>"
                + QString::number(balance, 'f', 2) + " €</b></p>";
        html += "<hr style='border: 1px solid #e0e0e0;'>";
        html += "<p style='color: #666; font-size: 12px; text-transform: uppercase;'><b>Transactions</b></p>";

        QJsonArray events = obj.value("events").toArray();
        if (events.isEmpty()) {
            html += "<p style='color: #999;'>No transactions this session.</p>";
        } else {
            for (const QJsonValue &v : events) {
                QJsonObject e = v.toObject();
                QString type = e.value("transaction_type").toString();
                QString amountStr = e.value("amount").toString();
                amountStr.replace(",", ".");
                double amount = amountStr.toDouble();

                QString tRaw = e.value("transaction_date").toString();
                QDateTime tdt = QDateTime::fromString(tRaw, "yyyy-MM-dd HH:mm:ss");
                QString tStr = tdt.isValid()
                                   ? tdt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                                   : tRaw;

                // цвет суммы: зелёный для DEPOSIT, красный для остальных
                QString color = (type == "DEPOSIT") ? "#2e7d32" : "#c62828";

                html += "<div style='display: flex; justify-content: space-between; "
                        "padding: 8px 0; border-bottom: 1px solid #f0f0f0;'>";
                html += "<span style='color: #333;'>" + tStr + "</span>";
                html += "<span style='color: #555; margin: 0 10px;'>" + type + "</span>";
                html += "<span style='color: " + color + "; font-weight: bold;'>"
                        + QString::number(amount, 'f', 2) + " €</span>";
                html += "</div>";
            }
        }

        html += "</div>";
        return html;
    }
*/

QString Receipt::buildReceiptText(const QJsonObject &obj) const
{
    QString timestampIso = obj.value("timestamp").toString();
    double balance = obj.value("balance").toDouble(0.0);

    QDateTime dt = QDateTime::fromString(timestampIso, Qt::ISODate);
    QString dtStr = dt.isValid()
                        ? dt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                        : QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");

    QString html;
    html += "<div style='font-family: Arial; padding: 10px;'>";
    html += "<p style='color: #000000; font-size: 24px; text-align: left;'>Date: <b>" + dtStr + "</b></p>";
    html += "<p style='color: #000000; font-size: 24px; text-align: center;'>Balance: <b style='color: #1a73e8;'>"
            + QString::number(balance, 'f', 2) + " €</b></p>";
    html += "<hr style='border: 1px solid #e0e0e0;'>";
    html += "<p style='color: #000000; font-size: 18px; margin-bottom: 16px;'><b>TRANSACTIONS</b></p>";

    QJsonArray events = obj.value("events").toArray();
    if (events.isEmpty()) {
        html += "<p style='color: #000000;'>No transactions this session.</p>";
    } else {
        html += "<table width='100%' cellspacing='0' cellpadding='6'>";
        for (const QJsonValue &v : events) {
            QJsonObject e = v.toObject();
            QString type = e.value("transaction_type").toString();
            QString amountStr = e.value("amount").toString();
            amountStr.replace(",", ".");
            double amount = amountStr.toDouble();

            QString tRaw = e.value("transaction_date").toString();
            QDateTime tdt = QDateTime::fromString(tRaw, "yyyy-MM-dd HH:mm:ss");
            QString tStr = tdt.isValid()
                               ? tdt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                               : tRaw;

            QString color = (type == "DEPOSIT") ? "#2e7d32" : "#c62828";

            html += "<tr>";
            html += "<td style='color: #000000; font-size: 18px;'>" + tStr + "</td>";
            html += "<td style='color: #000000; font-size: 18px;'>" + type + "</td>";
            html += "<td align='right' style='color: " + color + "; font-size: 18px; font-weight: bold;'>"
                    + QString::number(amount, 'f', 2) + " €</td>";
            html += "</tr>";
        }
        html += "</table>";
    }

    html += "</div>";
    return html;
}

void Receipt::on_btnBack_clicked()
{
    accept();
}

void Receipt::showError(const QString &msg)
{
    ui->txtReceipt->setPlainText("Virhe:\n" + msg);
}
