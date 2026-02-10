#include "debitvscredit.h"
#include "menu.h"
#include "ui_debitvscredit.h"


DebitvsCredit::DebitvsCredit(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DebitvsCredit)
{
    ui->setupUi(this);
    this->cardId = Environment::cardId;
    this->accountId = Environment::accountId;
    connect(ui->btnDebit, &QPushButton::clicked, this, &DebitvsCredit::DebitButtonClicked);
    connect(ui->btnCredit, &QPushButton::clicked, this, &DebitvsCredit::CreditButtonClicked);
}

DebitvsCredit::~DebitvsCredit()
{
    delete ui;
}

void DebitvsCredit::setToken(const QString &newToken)
{
    //tarkista
    token = "Bearer "+newToken;
    qDebug()<<token;
    qDebug() << "Current cardId: " << cardId;
    qDebug() << "Current accountId: " << accountId;
}

void DebitvsCredit::DebitButtonClicked()
{
    if(Environment::accountIds.size() > 0){
        Environment::accountId = Environment::accountIds[0];
        qDebug()<<"Debit side"<<Environment::accountId;
        Menu *m = new Menu(this);
        m->show();
        this->close();
    }
}

void DebitvsCredit::CreditButtonClicked()
{
    if(Environment::accountIds.size() > 1){
        Environment::accountId = Environment::accountIds[1];
        qDebug()<<"Credit side"<<Environment::accountId;
        Menu *m = new Menu(this);
        m->show();
        this->close();
    }
}
