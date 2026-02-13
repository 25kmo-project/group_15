#include "transactionhistory.h"
#include "ui_transactionhistory.h"
#include "environment.h"
#include <QHeaderView>
#include <QDebug>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

TransactionHistory::TransactionHistory(int accId, int cId, QString t, QWidget *parent) :
    QWidget(parent), ui(new Ui::TransactionHistory),
    accountId(accId), cardId(cId), token(t), currentPage(1)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);

    if (!ui->tableTransactions) {
        qDebug() << "ERROR: Table widget not found in UI!";
        return;
    }

    initTableStyle();
    displayData();
}

TransactionHistory::~TransactionHistory()
{
    delete ui;
}

void TransactionHistory::initTableStyle()
{
    QHeaderView *header = ui->tableTransactions->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive);

    header->setSectionResizeMode(0, QHeaderView::Stretch);

    header->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableTransactions->setColumnWidth(1, 100);

    header->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableTransactions->setColumnWidth(2, 100);

    header->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->tableTransactions->setColumnWidth(3, 60);

    ui->tableTransactions->verticalHeader()->setVisible(false);
}

void TransactionHistory::displayData()
{
    if (!ui->lblPageNumber) {
        qDebug() << "ERROR: lblPageNumber is NULL!";
        return;
    }

    ui->lblPageNumber->setText(QString("Page: %1").arg(currentPage));

    QString path = "transaction/history";
    QUrl url(Environment::base_url() + path);

    QUrlQuery query;
    query.addQueryItem("account_id", QString::number(accountId));
    query.addQueryItem("card_id", QString::number(cardId));
    query.addQueryItem("page", QString::number(currentPage));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QJsonArray jsonArray = doc.array();

            ui->tableTransactions->setRowCount(0);

            if (!ui->btnNextPage || !ui->btnPrevPage) {
                qDebug() << "ERROR: Navigation buttons not found!";
                reply->deleteLater();
                return;
            }

            ui->btnNextPage->setEnabled(jsonArray.size() >= 10);
            ui->btnPrevPage->setEnabled(currentPage > 1);

            for (const QJsonValue &value : jsonArray) {
                QJsonObject obj = value.toObject();
                int row = ui->tableTransactions->rowCount();
                ui->tableTransactions->insertRow(row);

                ui->tableTransactions->setItem(row, 0, new QTableWidgetItem(obj["Aika"].toString()));

                QTableWidgetItem *typeItem = new QTableWidgetItem(obj["Tyyppi"].toString());
                typeItem->setTextAlignment(Qt::AlignCenter);
                ui->tableTransactions->setItem(row, 1, typeItem);

                double amount = obj["Määrä"].toDouble();
                QTableWidgetItem *amountItem = new QTableWidgetItem(QString::number(amount, 'f', 2) + " €");
                amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                ui->tableTransactions->setItem(row, 2, amountItem);

                QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(obj["ID"].toInt()));
                idItem->setTextAlignment(Qt::AlignCenter);
                ui->tableTransactions->setItem(row, 3, idItem);
            }
        } else {
            qDebug() << "Error fetching transaction history:" << reply->errorString();
            ui->btnNextPage->setEnabled(false);
        }
        reply->deleteLater();
    });
}

void TransactionHistory::on_btnNextPage_clicked()
{
    currentPage++;
    displayData();
}

void TransactionHistory::on_btnPrevPage_clicked()
{
    if(currentPage > 1) {
        currentPage--;
        displayData();
    }
}

void TransactionHistory::on_btnBackToMenu_clicked()
{
    this->close();
}
