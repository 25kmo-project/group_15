#include "menu.h"
#include "ui_menu.h"
#include "deposit.h"
#include <QUrlQuery>
#include <QDebug>
#include "clientinfo.h"
#include "transactionhistory.h"
#include "transfer.h"


Menu::Menu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Menu)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->backButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
}

Menu::~Menu()
{
    delete ui;
}

void Menu::setupRequest(QNetworkRequest &request, const QString &path)
{
    request.setUrl(QUrl(Environment::base_url() + path));
    QByteArray authHeader = "Bearer " + Environment::token.toUtf8();
    request.setRawHeader("Authorization", authHeader);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
}

void Menu::on_btnDeposit_clicked()
{
    Deposit d(this);
    if (d.exec() == QDialog::Accepted) {
        on_btnBalance_clicked();
    }
}

void Deposit::on_btnBack_clicked()
{
    this->reject();  // closes deposit window
}


void Menu::on_btnBalance_clicked()
{
    QString path = "transaction/balance";
    QUrl url(Environment::base_url() + path);
    QUrlQuery query;
    query.addQueryItem("account_id", QString::number(Environment::accountId));
    query.addQueryItem("card_id", QString::number(Environment::cardId));
    url.setQuery(query);

    QNetworkRequest request(url);
    QByteArray authHeader = "Bearer " + Environment::token.toUtf8();
    request.setRawHeader("Authorization", authHeader);

    reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &Menu::onBalanceReceived);
}

void Menu::onBalanceReceived()
{
    ui->stackedWidget->setCurrentIndex(1);

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        double balance = jsonObj["balance"].toDouble();
        ui->labelBalance2->setText(QString::number(balance, 'f', 2) + " €");
    } else {
        qDebug() << "Error fetching balance:" << reply->errorString();
        ui->labelBalance2->setText("Error!");
    }

    reply->deleteLater();
}

void Menu::on_btnTransactionHistory_clicked()
{
    if (Environment::token.isEmpty()) {
        qDebug() << "Error: Token is empty, cannot open history";
        return;
    }

    TransactionHistory *historyWin = new TransactionHistory(
        Environment::accountId,
        Environment::cardId,
        Environment::token,
        nullptr
        );
    historyWin->setAttribute(Qt::WA_DeleteOnClose);

    connect(historyWin, &QWidget::destroyed, this, &Menu::show);

    historyWin->show();
    this->hide();
}

void Menu::on_btnTransfer_clicked()
{
    Transfer t(Environment::accountId, Environment::cardId, Environment::token, this);

    if (t.exec() == QDialog::Accepted) {
        on_btnBalance_clicked(); 
    }
}

void Menu::on_btnMyProfile_clicked()
{
    QString path = "user/" + QString::number(Environment::userId);
    QUrl url(Environment::base_url() + path);

    QNetworkRequest request(url);
    QByteArray authHeader = "Bearer " + Environment::token.toUtf8();
    request.setRawHeader("Authorization", authHeader);

    reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &Menu::onMyProfileReceived);
}

void Menu::onMyProfileReceived()
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();

        ClientInfo *clientInfoWin = new ClientInfo(this);
        clientInfoWin->setAttribute(Qt::WA_DeleteOnClose);
        clientInfoWin->setInfo(responseData);

        connect(clientInfoWin, &QWidget::destroyed, this, &Menu::show);

        clientInfoWin->show();
        this->hide();
    } else {
        qDebug() << "Error fetching profile:" << reply->errorString();
    }

    reply->deleteLater();
}
