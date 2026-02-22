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

// Escapes special HTML characters in a string
static QString esc(const QString &s) { return s.toHtmlEscaped(); }

// Returns color based on transaction type:
// green for deposits, red for withdrawals/transfers/negative values
static QString moneyColor(const QString& type, double amount)
{
    if (type == "DEPOSIT") return "#2e7d32";
    if (amount < 0) return "#c62828";
    if (type == "WITHDRAWAL" || type == "TRANSFER") return "#c62828";
    return "#111827";
}

// Helper to build a two-column activity row (label on left, timestamp on right)
static QString activityRow(const QString& label, const QString& time)
{
    return "<tr style='border-bottom:1px solid #f3f4f6;'>"
           "<td style='padding:10px 12px; width:60%; color:#111827;'>" + esc(label) + "</td>"
                          "<td style='padding:10px 12px; width:40%; text-align:right; color:#111827;'>" + esc(time) + "</td>"
                         "</tr>";
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

    // Make QTextEdit read-only and frameless (viewer mode)
    ui->txtReceipt->setReadOnly(true);
    ui->txtReceipt->setFrameStyle(QFrame::NoFrame);

    fetchReceipt();
}

Receipt::~Receipt()
{
    // Abort any pending network request on close
    if (reply) {
        reply->abort();
        reply->deleteLater();
        reply = nullptr;
    }
    delete ui;
}

void Receipt::fetchReceipt()
{
    // Check that session data is available
    if (Environment::token.isEmpty()) {
        showError("Token missing. Login again.");
        return;
    }

    if (Environment::cardId == 0 || Environment::accountId == 0) {
        showError("cardId or accountId missing. Select account or login again.");
        return;
    }

    // Try /transaction/receipt/full first; fall back to /transaction/receipt if 404
    const QString endpoint = triedFull ? "transaction/receipt"
                                       : "transaction/receipt/full";

    QUrl url(Environment::base_url() + endpoint);
    QUrlQuery query;
    query.addQueryItem("account_id", QString::number(Environment::accountId));
    query.addQueryItem("card_id", QString::number(Environment::cardId));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", ("Bearer " + Environment::token).toUtf8());

    // Cancel any existing request before starting a new one
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

    // If /receipt/full returns 404, retry with the basic /receipt endpoint
    if (netErr != QNetworkReply::NoError) {
        if (!triedFull && httpStatus == 404) {
            triedFull = true;
            fetchReceipt();
            return;
        }
        showError("Could not fetch receipt: " + errStr);
        return;
    }

    // Parse JSON response
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
    // --- Parse timestamp and balance (always shown) ---
    const QString timestampIso = obj.value("timestamp").toString();
    const double balance = obj.value("balance").toDouble(0.0);

    QDateTime dt = QDateTime::fromString(timestampIso, Qt::ISODate);
    const QString dtStr = dt.isValid()
                              ? dt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                              : QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");

    // --- Parse transaction events ---
    const QJsonArray events = obj.value("events").toArray();

    // Check if there is any session activity to show
    const bool hasActivity = Environment::viewedBalance
                             || Environment::viewedProfile
                             || Environment::viewedTransactionHistory
                             || Environment::viewedCurrency;

    // ===== Build HTML =====
    QString html;

    html += R"(
<div style="font-family: Arial; background:#f5f7fa; padding: 18px;">
  <div style="text-align:center; font-size:48px; font-weight:800; color:#0f172a; margin: 10px 0 22px 0;">
    Digital Receipt
  </div>
  <div style="max-width: 760px; margin: 0 auto; background:#ffffff; border:1px solid #e5e7eb; border-radius: 12px; padding: 18px;">
)";

    // --- Header: date and balance (always visible) ---
    html += "<div style='display:flex; justify-content:space-between; align-items:flex-end; gap:16px; flex-wrap:wrap;'>";
    html += "<div style='font-size:22px; color:#111827;'>Date: <b>" + esc(dtStr) + "</b></div>";
    html += "<div style='font-size:24px; color:#111827;'>Balance: "
            "<b style='color:#1a73e8; font-size:26px;'>" + QString::number(balance,'f',2) + " €</b></div>";
    html += "</div>";

    html += "<hr style='border:none; border-top:1px solid #e5e7eb; margin:16px 0;'>";

    // --- Session activity: shown only if user visited any screen ---
    if (hasActivity) {
        html += "<div style='font-size:13px; font-weight:700; color:#111827; "
                "text-transform:uppercase; letter-spacing:0.8px; margin-bottom:10px;'>Session Activity</div>";

        html += "<div style='border:1px solid #e5e7eb; border-radius:10px; overflow:hidden; margin-bottom:16px;'>";
        html += "<table style='width:100%; border-collapse:collapse; font-size:15px; color:#111827;'>";

        if (Environment::viewedBalance)
            html += activityRow("Balance inquiry",
                                Environment::timeViewedBalance.toString("dd.MM.yyyy HH:mm:ss"));
        if (Environment::viewedProfile)
            html += activityRow("My profile viewed",
                                Environment::timeViewedProfile.toString("dd.MM.yyyy HH:mm:ss"));
        if (Environment::viewedTransactionHistory)
            html += activityRow("Transaction history viewed",
                                Environment::timeViewedTransactionHistory.toString("dd.MM.yyyy HH:mm:ss"));
        if (Environment::viewedCurrency)
            html += activityRow("Currency rates viewed",
                                Environment::timeViewedCurrency.toString("dd.MM.yyyy HH:mm:ss"));

        html += "</table></div>";
    }

    // --- Transactions: shown only if there are events in this session ---
    if (!events.isEmpty()) {
        html += "<div style='font-size:13px; font-weight:700; color:#111827; "
                "text-transform:uppercase; letter-spacing:0.8px; margin-bottom:10px;'>Transactions</div>";

        html += "<div style='border:1px solid #e5e7eb; border-radius:10px; overflow:hidden;'>";
        html += "<table style='width:100%; border-collapse:collapse; font-size:15px; color:#111827;'>";

        for (const QJsonValue &v : events) {
            const QJsonObject e = v.toObject();
            const QString type = e.value("transaction_type").toString();

            // Parse amount (handle both string and number formats)
            QString amountStr = e.value("amount").toString();
            amountStr.replace(",", ".");
            const double amount = amountStr.toDouble();

            // Format transaction date
            const QString tRaw = e.value("transaction_date").toString();
            const QDateTime tdt = QDateTime::fromString(tRaw, "yyyy-MM-dd HH:mm:ss");
            const QString tStr = tdt.isValid()
                                     ? tdt.toLocalTime().toString("dd.MM.yyyy HH:mm:ss")
                                     : tRaw;

            const QString col = moneyColor(type, amount);

            html += "<tr style='border-bottom:1px solid #f3f4f6;'>";
            html += "<td style='padding:10px 12px; width:40%; color:#111827;'>" + esc(tStr) + "</td>";
            html += "<td style='padding:10px 12px; width:35%; font-weight:600;'>" + esc(type) + "</td>";
            html += "<td style='padding:10px 12px; width:25%; text-align:right; font-weight:800; color:" + col + ";'>"
                    + esc(QString::number(amount, 'f', 2) + " €") + "</td>";
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

// Back button — close the dialog and return to menu
void Receipt::on_btnBack_clicked()
{
    accept();
}

// Display an error message in the receipt viewer
void Receipt::showError(const QString &msg)
{
    ui->txtReceipt->setPlainText("Error:\n" + msg);
}
