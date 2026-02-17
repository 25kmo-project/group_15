#include <QUrlQuery>
#include <QDebug>
#include <QApplication>
#include "clientinfo.h"
#include "transactionhistory.h"
#include "transfer.h"
#include "mainwindow.h"
#include "menu.h"
#include "ui_menu.h"
#include "deposit.h"
#include "withdraw.h"

#include "balance.h"


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

//deposit
void Menu::on_btnDeposit_clicked()
{
    Deposit d(this);
    if (d.exec() == QDialog::Accepted) {
        on_btnBalance_clicked();
    }
}

void Menu::on_btnWithdraw_clicked()
{
    Withdraw w(this);

    if (w.exec() == QDialog::Accepted) {
        // Päivitä saldo jos nosto onnistunut
        on_btnBalance_clicked();
    }
}

//balance
void Menu::on_btnBalance_clicked()
{
    Balance *balanceWin = new Balance(this);
    balanceWin->setAttribute(Qt::WA_DeleteOnClose);

    connect(balanceWin, &QWidget::destroyed, this, &Menu::show);

    balanceWin->show();
    this->hide();
}

//transaction history
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
//transfer
void Menu::on_btnTransfer_clicked()
{
    Transfer *transferWin = new Transfer(Environment::accountId, Environment::cardId, Environment::token, this);
    transferWin->setAttribute(Qt::WA_DeleteOnClose);

    connect(transferWin, &QDialog::accepted, this, [this](){
        on_btnBalance_clicked();
    });

    connect(transferWin, &QWidget::destroyed, this, &Menu::show);

    transferWin->show();
    this->hide();

}
//currency
void Menu::on_btnCurrency_clicked()
{
    if (Environment::token.isEmpty()) {
        qDebug() << "Error: Token is empty, cannot open currency";
        return;
    }

    Currency c(Environment::token, this);
    c.exec();
}
//my profile
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
//log out
void Menu::on_btnLogOut_clicked()
{
        //clear client's data
        Environment::token = "";
        Environment::userId = 0;
        Environment::cardId = 0;
        Environment::accountId = 0;
        Environment::accountIds.clear();

        qDebug() << "Log out";

        //creating new mainwindow
        MainWindow *newLoginWindow = new MainWindow();
        newLoginWindow->show();

        //close current window
        this->close();
}
