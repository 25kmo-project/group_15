#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "environment.h"
#include "debitvscredit.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //Clear fields (card id and password)
    void clearFields();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
private slots:
    void btnLoginSlot();
    void loginAction();
};
#endif // MAINWINDOW_H
