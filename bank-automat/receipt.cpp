#include "receipt.h"
#include "ui_receipt.h"
#include "environment.h"

#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDateTime>
#include <QNetworkRequest>
#include <QDebug>

static QString esc(const QString &s) { return s.toHtmlEscaped(); }

static QString moneyColor(const QString& type, double amount)
{
    // Green for deposits, red for withdrawals/transfers/negative values
    if (type == "DEPOSIT") return "#2e7d32";
    if (amount < 0) return "#c62828";
    if (type == "WITHDRAWAL" || type == "TRANSFER") return "#c62828";
    return "#111827";
}

static QString row2(const QString& left, const QString& right)
{
    return "<div style='display:flex; justify-content:space-between; gap:12px; padding:6px 0;'>"
           "<div style='color:#111827;'>" + esc(left) + "</div>"
                         "<div style='color:#111827; font-weight:700; text-align:right;'>" + esc(right) + "</div>"
                          "</div>";
}

Receipt::Receipt(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Receipt),
    networkManager(new QNetworkAccessManager(this)),
    reply(nullptr),
    triedFull(false)
{
    ui->setupUi(this);
    setWindowTitle("");

    // Make QTextEdit behave like a viewer
    ui->txtReceipt->setReadOnly(true);
    ui->txtReceipt->setFrameStyle(QFrame::NoFrame);

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
        showError("Token missing. Login again.");
        return;
    }

    if (Environment::cardId == 0 || Environment::accountId == 0) {
        showError("cardId or accountId missing. Select account or login again.");
        return;
    }

    // Try /receipt/full first; if backend doesn't have it -> fallback to /receipt
    const QString endpoint = triedFull ? "transaction/receipt"
                                       : "transaction/receipt/full";

    QUrl url(Environment::base_url() + endpoint);
    QUrlQuery query;
    query.addQueryItem("account_id", QString::number(Environment::accountId));
    query.addQueryItem("card_id", QString::number(Environment::cardId));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", ("Bearer " + Environment::token).toUtf8());

    if (reply) {
        reply->abort();
        reply->deleteLater();
        reply = nullptr;
    }

    reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &Receipt::onReceiptReceived);

    ui->txtReceipt->setPlainText("Fetching receipt...");
}

void Receipt::onReceiptReceived()
{
    if (!reply) return;

    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QNetworkReply::NetworkError netErr = reply->error();
    const QString errStr = reply->errorString();
    const QByteArray responseData = reply->readAll();

    reply->deleteLater();
    reply = nullptr;

    // If /receipt/full returns 404 -> retry with /receipt
    if (netErr != QNetworkReply::NoError) {
        if (!triedFull && httpStatus == 404) {
            triedFull = true;
            fetchReceipt();
            return;
        }

        showError("Could not fetch receipt: " + errStr);
        return;
    }

    QJsonParseError pe;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &pe);
    if (!doc.isObject()) {
        showError("Invalid response from backend (JSON).");
        return;
    }

    const QJsonObject obj = doc.object();
    ui->txtReceipt->setHtml(buildReceiptHtml(obj));
}

QString Receipt::buildReceiptHtml(const QJsonObject &obj) const
{
    // ---- Basic fields ----
    const QString timestampIso = obj.value("timestamp").toString();
    const double balance = obj.value("balance").toDouble(0.0);

    QDateTime dt = QDateTime::fromString(timestampIso, Qt::ISODate);
    const QString dtStr = dt.isValid()
                              ? dt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                              : QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");

    // ---- Profile (optional) ----
    const QJsonValue profileVal = obj.value("profile");
    const QJsonObject profile = profileVal.isObject() ? profileVal.toObject() : QJsonObject();

    const QString first = profile.value("user_name").toString();
    const QString last  = profile.value("user_lastname").toString();
    const QString addr  = profile.value("user_address").toString();
    const QString email = profile.value("user_email").toString();
    const QString phone = profile.value("user_phonenumber").toString();

    // ---- Currency (optional) ----
    const QJsonValue currencyVal = obj.value("currency");
    const QJsonObject currency = currencyVal.isObject() ? currencyVal.toObject() : QJsonObject();

    const QJsonObject latest = currency.value("latest").toObject();
    const QJsonObject change = currency.value("change").toObject();

    // ---- Events ----
    const QJsonArray events = obj.value("events").toArray();

    // ===== HTML start =====
    QString html;

    html += R"(
<div style="font-family: Arial; background:#f5f7fa; padding: 18px;">
  <div style="text-align:center; font-size:48px; font-weight:800; color:#0f172a; margin: 10px 0 22px 0;">
    Digital Receipt
  </div>

  <div style="max-width: 760px; margin: 0 auto; background:#ffffff; border:1px solid #e5e7eb; border-radius: 12px; padding: 18px;">
)";

    // Header row (date + balance)
    html += "<div style='display:flex; justify-content:space-between; align-items:flex-end; gap:16px; flex-wrap:wrap;'>";
    html += "<div style='font-size:22px; color:#111827;'>Date: <b>" + esc(dtStr) + "</b></div>";
    html += "<div style='font-size:24px; color:#111827;'>Balance: "
            "<b style='color:#1a73e8; font-size:26px;'>" + QString::number(balance,'f',2) + " €</b></div>";
    html += "</div>";

    html += "<hr style='border:none; border-top:1px solid #e5e7eb; margin:16px 0;'>";

    // ===== Customer info card =====
    html += "<div style='font-size:18px; font-weight:800; color:#111827; margin-bottom:8px;'>CUSTOMER INFORMATION</div>";
    html += "<div style='background:#f8fafc; border:1px solid #e5e7eb; border-radius:10px; padding:12px; margin-bottom:14px;'>";

    if (profile.isEmpty()) {
        html += "<div style='color:#374151;'>No customer information available.</div>";
    } else {
        if (!first.isEmpty() || !last.isEmpty())
            html += row2("Name", first + " " + last);
        if (!addr.isEmpty())
            html += row2("Address", addr);
        if (!email.isEmpty())
            html += row2("Email", email);
        if (!phone.isEmpty())
            html += row2("Phone number", phone);
    }
    html += "</div>";

    // ===== Exchange rates latest =====
    html += "<div style='font-size:18px; font-weight:800; color:#111827; margin-bottom:8px;'>EXCHANGE RATES (LATEST)</div>";
    html += "<div style='background:#f8fafc; border:1px solid #e5e7eb; border-radius:10px; padding:12px; margin-bottom:14px;'>";

    if (latest.isEmpty()) {
        html += "<div style='color:#374151;'>No exchange rate data available.</div>";
    } else {
        const QString base = latest.value("base").toString("EUR");
        const QString date = latest.value("date").toString();
        const double usd = latest.value("USD").toDouble(0.0);
        const double gbp = latest.value("GBP").toDouble(0.0);

        html += row2("Base", base);
        if (!date.isEmpty()) html += row2("Date", date);
        if (usd > 0) html += row2("USD", QString::number(usd, 'f', 6));
        if (gbp > 0) html += row2("GBP", QString::number(gbp, 'f', 6));
    }
    html += "</div>";

    // ===== Exchange rate change =====
    html += "<div style='font-size:18px; font-weight:800; color:#111827; margin-bottom:8px;'>EXCHANGE RATE CHANGE</div>";
    html += "<div style='background:#f8fafc; border:1px solid #e5e7eb; border-radius:10px; padding:12px; margin-bottom:14px;'>";

    if (change.isEmpty()) {
        html += "<div style='color:#374151;'>No change data available.</div>";
    } else {
        const QString base = change.value("base").toString("EUR");
        const QString prevDate = change.value("prevDate").toString();
        const QString lastDate = change.value("lastDate").toString();

        html += row2("Base", base);
        if (!prevDate.isEmpty() && !lastDate.isEmpty())
            html += row2("Period", prevDate + " → " + lastDate);

        auto addSymbol = [&](const QString &sym){
            if (!change.value(sym).isObject()) return;
            const QJsonObject s = change.value(sym).toObject();

            const double rate = s.value("rate").toDouble(0.0);
            const double abs  = s.value("abs").toDouble(0.0);
            const double pct  = s.value("pct").toDouble(0.0);

            QString right = QString::number(rate, 'f', 6)
                            + " (" + (abs >= 0 ? "+" : "") + QString::number(abs, 'f', 6)
                            + ", " + (pct >= 0 ? "+" : "") + QString::number(pct, 'f', 2) + "%)";

            html += row2(sym, right);
        };

        addSymbol("USD");
        addSymbol("GBP");
    }
    html += "</div>";

    // ===== Transactions =====
    html += "<div style='font-size:18px; font-weight:800; letter-spacing:0.5px; color:#111827; margin-bottom:8px;'>TRANSACTIONS</div>";

    if (events.isEmpty()) {
        html += "<div style='color:#374151; font-size:16px;'>No events during this session.</div>";
    } else {
        html += "<div style='border:1px solid #e5e7eb; border-radius:10px; padding:12px; background:#ffffff;'>";
        html += "<table style='width:100%; border-collapse:collapse; font-size:16px; color:#111827;'>";

        for (const QJsonValue &v : events) {
            const QJsonObject e = v.toObject();
            const QString type = e.value("transaction_type").toString();

            QString amountStr = e.value("amount").toString();
            amountStr.replace(",", ".");
            const double amount = amountStr.toDouble();

            const QString tRaw = e.value("transaction_date").toString();
            const QDateTime tdt = QDateTime::fromString(tRaw, "yyyy-MM-dd HH:mm:ss");
            const QString tStr = tdt.isValid()
                                     ? tdt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                                     : tRaw;

            const QString col = moneyColor(type, amount);
            const QString amountShown = QString::number(amount, 'f', 2) + " €";

            html += "<tr>";
            html += "<td style='padding:6px 0; width:45%;'>" + esc(tStr) + "</td>";
            html += "<td style='padding:6px 0; width:35%;'>" + esc(type) + "</td>";
            html += "<td style='padding:6px 0; width:20%; text-align:right; font-weight:800; color:" + col + ";'>"
                    + esc(amountShown) + "</td>";
            html += "</tr>";
        }

        html += "</table></div>";
    }

    html += R"(
  </div>
</div>
)";

    return html;
}

void Receipt::on_btnBack_clicked()
{
    accept();
}

void Receipt::showError(const QString &msg)
{
    ui->txtReceipt->setPlainText("Error:\n" + msg);
}
