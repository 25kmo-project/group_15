#include "debitvscredit.h"
#include "menu.h"
#include "ui_debitvscredit.h"

#include <QPointer>


DebitvsCredit::DebitvsCredit(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DebitvsCredit)
{
    ui->setupUi(this);

    setWindowTitle("Card choice");

    this->cardId = Environment::cardId;
    this->accountId = Environment::accountId;

    // start timer when window opens
    if (!Environment::timerLogOut) {
        Environment::timerLogOut = new QTimer();
        Environment::timerLogOut->setInterval(30000);
    }
    connect(Environment::timerLogOut, &QTimer::timeout, this, &DebitvsCredit::autoLogOut);
    Environment::timerLogOut->start();

    connect(ui->btnDebit, &QPushButton::clicked, this, &DebitvsCredit::DebitButtonClicked);
    connect(ui->btnCredit, &QPushButton::clicked, this, &DebitvsCredit::CreditButtonClicked);
}

DebitvsCredit::~DebitvsCredit()
{
    if (Environment::timerLogOut) {
        disconnect(Environment::timerLogOut, &QTimer::timeout, this, &DebitvsCredit::autoLogOut);
    }
    delete ui;
}

void DebitvsCredit::setToken(const QString &newToken)
{
    //check
    token = "Bearer "+newToken;
    qDebug()<<token;
    qDebug() << "Current cardId: " << cardId;
    qDebug() << "Current accountId: " << accountId;
}

void DebitvsCredit::DebitButtonClicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    if(Environment::accountIds.size() > 0){
        Environment::accountId = Environment::accountIds[0];
        qDebug()<<"Debit side"<<Environment::accountId;
        Menu *m = new Menu(nullptr);
        m->show();
        this->close();
    }
}

void DebitvsCredit::CreditButtonClicked()
{
    //restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }

    if(Environment::accountIds.size() > 1){
        Environment::accountId = Environment::accountIds[1];
        qDebug()<<"Credit side"<<Environment::accountId;
        Menu *m = new Menu(nullptr);
        m->show();
        this->close();
    }
}

void DebitvsCredit::autoLogOut()
{
    qDebug() << "DebitvsCredit autoLogOut";

    if (Environment::timerLogOut) {
        Environment::timerLogOut->stop();
    }

    // clear data
    Environment::token = "";
    Environment::userId = 0;
    Environment::cardId = 0;
    Environment::accountId = 0;
    Environment::accountIds.clear();

    // find or create MainWindow
    MainWindow *existingMainWindow = nullptr;
    for (QWidget *w : QApplication::topLevelWidgets()) {
        if (qobject_cast<MainWindow*>(w)) {
            existingMainWindow = qobject_cast<MainWindow*>(w);
            break;
        }
    }

    if (existingMainWindow) {
        existingMainWindow->clearFields();
        existingMainWindow->show();
        existingMainWindow->raise();
        existingMainWindow->activateWindow();
    } else {
        MainWindow *newLoginWindow = new MainWindow();
        newLoginWindow->show();
        existingMainWindow = newLoginWindow;
    }

    QPointer<MainWindow> safeMain = existingMainWindow;
    QPointer<DebitvsCredit> safeThis = this;
    QTimer::singleShot(300, qApp, [safeMain, safeThis]() {
        for (QWidget *w : QApplication::topLevelWidgets()) {
            if (qobject_cast<MainWindow*>(w)) continue;
            w->hide();
            w->deleteLater();
        }
    });
}
