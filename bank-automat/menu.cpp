#include "menu.h"
#include "ui_menu.h"
#include "deposit.h"
#include <QUrlQuery>
#include <QDebug>
#include "transactionhistory.h"


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

// Base URL、Token auth and Content-Type
void Menu::setupRequest(QNetworkRequest &request, const QString &path)
{
    request.setUrl(QUrl(Environment::base_url() + path));

    // get JWT Token when login
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
    //1.1. Construct the URL with required query parameters
    QString path = "transaction/balance";
    QUrl url(Environment::base_url() + path);
    QUrlQuery query;
    query.addQueryItem("account_id", QString::number(Environment::accountId));
    query.addQueryItem("card_id", QString::number(Environment::cardId));
    url.setQuery(query);

    // 2.Prepare the network request and set Authorization headers
    QNetworkRequest request(url);
    QByteArray authHeader = "Bearer " + Environment::token.toUtf8();
    request.setRawHeader("Authorization", authHeader);

    qDebug() << "Fetching balance for Account:" << Environment::accountId;

    // 3.Send the asynchronous GET request to the backend
    reply = networkManager->get(request);

    // 4.Connect the finished signal to the response handler slot
    connect(reply, &QNetworkReply::finished, this, &Menu::onBalanceReceived);
}

void Menu::onBalanceReceived()
{
     ui->stackedWidget->setCurrentIndex(1);
    // 1.Check if the network response was successful
    if (reply->error() == QNetworkReply::NoError) {
        // Read the raw response data and parse it into a JSON document
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        // Extract values from the JSON object
        double balance = jsonObj["balance"].toDouble();
        QString owner = jsonObj["owner"].toString();

        qDebug() << "Balance received:" << balance;
        // Update the UI Label
        ui->labelBalance2->setText(QString::number(balance, 'f', 2) + " €");


    } else {
        // 2.Handle network errors or server-side failures
        qDebug() << "Error fetching balance:" << reply->errorString();
        ui->labelBalance2->setText("Error!");
    }

    reply->deleteLater();
}

void Menu::on_btnTransactionHistory_clicked() {
    if (Environment::token.isEmpty()) {
        qDebug() << "Error: Token is empty, cannot open history";
        return;
    }
    
    // new windows
    TransactionHistory *historyWin = new TransactionHistory(
        Environment::accountId, 
        Environment::cardId, 
        Environment::token, 
        this
    );
    historyWin->setAttribute(Qt::WA_DeleteOnClose); 
    
    connect(historyWin, &QWidget::destroyed, this, &Menu::show);
    
    historyWin->show();
}