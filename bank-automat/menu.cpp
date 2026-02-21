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
#include "receipt.h"


Menu::Menu(QWidget *parent): QDialog(parent), ui(new Ui::Menu)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);

    //create new timer if it does not exist
    if (!Environment::timerLogOut){
        Environment::timerLogOut = new QTimer();
        Environment::timerLogOut->setInterval(30000); //timer 30 seconds
    }

    //connecting autoLogOut method
    connect(Environment::timerLogOut, &QTimer::timeout, this, &Menu::autoLogOut);

    //timer starts now
    Environment::timerLogOut->start();
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
    //timer restart
    Environment::timerLogOut->start();

    Deposit *depositWin = new Deposit(this);
    depositWin->setAttribute(Qt::WA_DeleteOnClose);
    connect(depositWin, &QWidget::destroyed, this, &Menu::show);

    depositWin->show();

    this->hide();
}
//withdrawal
void Menu::on_btnWithdrawal_clicked()
{
    //timer restart
    Environment::timerLogOut->start();

    Withdraw *withdrawWin = new Withdraw(this);
    withdrawWin->setAttribute(Qt::WA_DeleteOnClose);

    connect(withdrawWin, &QWidget::destroyed, this, &Menu::show);

    withdrawWin->show();
    this->hide();
}
//receipt
void Menu::on_btnReceipt_clicked()
{
    Environment::timerLogOut->start();

    Receipt dlg(this);
    dlg.exec();
}
//balance
void Menu::on_btnBalance_clicked()
{
    //timer restart
    Environment::timerLogOut->start();

    Balance *balanceWin = new Balance(this);
    balanceWin->setAttribute(Qt::WA_DeleteOnClose);

    connect(balanceWin, &QWidget::destroyed, this, &Menu::show);

    balanceWin->show();
    this->hide();
}

//transaction history
void Menu::on_btnTransactionHistory_clicked()
{
    //timer restart
    Environment::timerLogOut->start();

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
    //timer restart
    Environment::timerLogOut->start();

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
    //timer restart
    Environment::timerLogOut->start();

    if (Environment::token.isEmpty()) {
        qDebug() << "Error: Token is empty, cannot open currency";
        return;
    }

    Currency *currencyWin = new Currency(Environment::token, this);
    currencyWin->setAttribute(Qt::WA_DeleteOnClose);

    connect(currencyWin, &QWidget::destroyed, this, &Menu::show);

    currencyWin->show();
    this->hide();
}



//my profile
void Menu::on_btnMyProfile_clicked()
{
    //timer restart
    Environment::timerLogOut->start();

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
    //timer is stopped
    if (Environment::timerLogOut) {
        Environment::timerLogOut->stop();
    }

    //close all windows
    QWidgetList allWindows = QApplication::topLevelWidgets();
    for (QWidget *w : allWindows) {
        if (w != this && w->isVisible()) {
            w->close();
        }
    }

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
void Menu::closeAllNotMain(MainWindow *mainWindow)
{
    QWidgetList allWindows = QApplication::topLevelWidgets();
    for (int i = 0; i < allWindows.size(); i++) {
        QWidget *window = allWindows[i];
        if (window != mainWindow) {
            window->deleteLater();
        }
    }
}


//automatic log out when inactivity is 30 seconds
void Menu::autoLogOut()
{
    qDebug() << "30 seconds passed, user is logged out";

    //timer stopped
    if (Environment::timerLogOut) {
        Environment::timerLogOut->stop();
    }

    //clear all data
    Environment::token = "";
    Environment::userId = 0;
    Environment::cardId = 0;
    Environment::accountId = 0;
    Environment::accountIds.clear();

    //find MainWindow
    QWidgetList allWindows = QApplication::topLevelWidgets();
    MainWindow *existingMainWindow = nullptr;

    for (int i = 0; i < allWindows.size(); i++) {
        if (qobject_cast<MainWindow*>(allWindows[i])) {
            existingMainWindow = qobject_cast<MainWindow*>(allWindows[i]);
            break;
        }
    }

    //show MainWindow and clear fields
    if (existingMainWindow) {
        existingMainWindow->clearFields();
        existingMainWindow->show();
        existingMainWindow->raise();
        existingMainWindow->activateWindow();
    } else {
        MainWindow *newLoginWindow = new MainWindow();
        newLoginWindow->show();
    }

    //delete all windows except MainWindow with delay so the app is not crushing
    QTimer::singleShot(0,this, [this, existingMainWindow]() {
        closeAllNotMain(existingMainWindow);
    });
    qDebug() << "Auto logout complete";
}
