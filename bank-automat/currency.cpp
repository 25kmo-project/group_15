#include "currency.h"
#include "ui_currency.h"
#include "environment.h"

#include <QJsonDocument>
#include <QJsonObject>

Currency::Currency(QString token, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Currency),
    manager(new QNetworkAccessManager(this)),
    reply(nullptr),
    m_token(token)
{
    ui->setupUi(this);

    connect(ui->btnBack, &QPushButton::clicked, this, &Currency::on_btnBack_clicked);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &Currency::fetchRates);


    // hae kurssit heti kun ikkuna avataan
    fetchRates();
}

Currency::~Currency()
{
    delete ui;
}

void Currency::setupRequest(QNetworkRequest &request, const QString &path)
{
    const QString url = Environment::base_url() + path;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization",
                             ("Bearer " + m_token).toUtf8());
    }
}

void Currency::fetchRates()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    QNetworkRequest request;
    setupRequest(request, "currency/change");

    reply = manager->get(request);
    connect(reply, &QNetworkReply::finished,this, &Currency::onRatesReceived);
}

void Currency::onRatesReceived()
{
    if (reply->error() == QNetworkReply::NoError) {

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        QString lastDate = obj.value("lastDate").toString();
        QString prevDate = obj.value("prevDate").toString();

        QJsonObject usdObj = obj.value("USD").toObject();
        QJsonObject gbpObj = obj.value("GBP").toObject();

        double usdRate = usdObj.value("rate").toDouble();
        double usdAbs  = usdObj.value("abs").toDouble();
        double usdPct  = usdObj.value("pct").toDouble();

        double gbpRate = gbpObj.value("rate").toDouble();
        double gbpAbs  = gbpObj.value("abs").toDouble();
        double gbpPct  = gbpObj.value("pct").toDouble();

        auto arrow = [](double abs) {
            if (abs > 0) return QString("↑");
            if (abs < 0) return QString("↓");
            return QString("→");
        };

        auto sign = [](double x) {
            return (x > 0) ? QString("+") : QString("");
        };

        // isot kurssit (vain numero)
        ui->labelUsdValue->setText(QString::number(usdRate, 'f', 4));
        ui->labelGbpValue->setText(QString::number(gbpRate, 'f', 4));

        // pienet muutokset erikseen
        ui->labelUsdChange->setText(
            QString("%1%2 (%3%4%) %5 vs %6")
                .arg(sign(usdAbs))
                .arg(QString::number(usdAbs, 'f', 4))
                .arg(sign(usdPct))
                .arg(QString::number(usdPct, 'f', 2))
                .arg(arrow(usdAbs))
                .arg(prevDate)
            );

        ui->labelGbpChange->setText(
            QString("%1%2 (%3%4%) %5 vs %6")
                .arg(sign(gbpAbs))
                .arg(QString::number(gbpAbs, 'f', 4))
                .arg(sign(gbpPct))
                .arg(QString::number(gbpPct, 'f', 2))
                .arg(arrow(gbpAbs))
                .arg(prevDate)
            );

        ui->labelDate->setText("Rates date: " + lastDate);
        ui->labelStatus->setText("");

    } else {
        ui->labelStatus->setText("Error: " + reply->errorString());

        ui->labelUsdValue->setText("-");
        ui->labelGbpValue->setText("-");
        ui->labelUsdChange->setText("");
        ui->labelGbpChange->setText("");
        ui->labelDate->setText("Rates date: -");
    }

    reply->deleteLater();
}

void Currency::on_btnBack_clicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }
    this->close();
}
