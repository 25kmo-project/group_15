#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "menu.h"
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Bank App");

    //code starts for ui shadow
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30); //shadow softness
    shadow->setOffset(0, 4); //depth
    shadow->setColor(QColor(0, 0, 0, 50)); //transparancy
    ui->loginCard->setGraphicsEffect(shadow);
    //the end od ui shadow

    //code starts for ui shadow 2 (additional layer)
    auto *shadow2 = new QGraphicsDropShadowEffect(this);
    shadow2->setBlurRadius(15);
    shadow2->setOffset(0, 1);
    shadow2->setColor(QColor(0, 0, 0, 20));
    ui->loginCard->graphicsEffect()->setParent(shadow2);
    ui->loginCard->setGraphicsEffect(shadow2);
    //the end of ui shadow 2

    //login
    connect(ui->btnLogin, &QPushButton::clicked, this, &MainWindow::btnLoginSlot);


    manager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearFields()
{
    ui->textCardId->clear();
    ui->textPincode->clear();
    ui->labelInfo->clear();
}

//login
void MainWindow::btnLoginSlot()
{
    QString url = Environment::base_url()+"login";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jObject;
    jObject.insert("card_id", ui->textCardId->text());
    jObject.insert("pin_code", ui->textPincode->text());
    QJsonDocument jsonDoc(jObject);
    reply = manager->post(request, jsonDoc.toJson());
    connect(reply, &QNetworkReply::finished, this, &MainWindow::loginAction);
}
//login
void MainWindow::loginAction()
{
    QByteArray responseData=reply->readAll();
    reply->deleteLater();

    //if backend is not working
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if(!jsonDoc.isObject()) {
        ui->labelInfo->setText("Sorry, we have some technical issues");
        //qDebug() << "Problems with backend" << responseData;
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    //show server message if exists
    if(jsonObject.contains("message")) {
        QString message = jsonObject["message"].toString();
        ui->labelInfo->setText(message);
        //qDebug() << "Server message:" << message;
    }

    //server sends token ja card_id qt konsoliin
    //DebitvsCredit window settings
    if(jsonObject.contains("token")) {
        Environment:: token = jsonObject["token"].toString();
        Environment:: cardId = jsonObject["card_id"].toInt();
        Environment::userId = jsonObject["user_id"].toInt();
        //clear previous accounts and load new ones
        Environment::accountIds.clear();
        QJsonArray accounts = jsonObject["account_id"].toArray();
        for (const QJsonValue &value : accounts) {
            Environment::accountIds.append(value.toInt());
        }
        qDebug() << "Total accounts found:" << Environment::accountIds.size();
        qDebug() << "login ok";
        qDebug() << "cardid:" << Environment::cardId;
        qDebug() << "userId:" << Environment::userId;
        qDebug() << "accountid:" << Environment::accountIds;
        qDebug() << "token:" << Environment::token;

        // SINGLE: straight to menu
        if (Environment::accountIds.size() == 1) {
            Environment::accountId = Environment::accountIds.first();

            if (jsonObject.contains("account_types") && jsonObject["account_types"].isArray()) {
                QJsonArray types = jsonObject["account_types"].toArray();
                if (!types.isEmpty()) {
                    Environment::cardType = types.first().toString().toUpper();
                } else {
                    Environment::cardType = "";
                }
            } else {
                Environment::cardType = "";
            }

            Menu *m = new Menu(this);
            m->show();
            this->hide();
        }
        // DUAL: DebitvsCredit then Menu
        else if (Environment::accountIds.size() > 1) {
            DebitvsCredit *objDebitvsCredit = new DebitvsCredit(this);
            objDebitvsCredit->setToken(Environment::token);
            objDebitvsCredit->show();
            this->hide();
        }
        else {
            ui->labelInfo->setText("No accounts linked to this card.");
        }
    }



    }

