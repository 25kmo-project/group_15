#include "debitvscredit.h"
#include "ui_debitvscredit.h"


DebitvsCredit::DebitvsCredit(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DebitvsCredit)
{
    ui->setupUi(this);
    this->cardId = Environment::cardId;
    this->accountId = Environment::accountId;
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
