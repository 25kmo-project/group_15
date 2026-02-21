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
    setWindowTitle("Transaction History");
    manager = new QNetworkAccessManager(this);

    if (!ui->tableTransactions) {
        qDebug() << "Table widget not found in UI!";
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
    // Table styling
    ui->tableTransactions->setStyleSheet(
        "QTableWidget {"
        "  border: 1px solid #e0e0e0;"
        "  border-radius: 6px;"
        "  background-color: #ffffff;"
        "  gridline-color: #f0f0f0;"
        "}"
        "QTableWidget::item {"
        "  padding: 8px;"
        "  color: #000000;"
        "  border-bottom: 1px solid #f0f0f0;"
        "}"
        "QTableWidget::item:selected {"
        "  background-color: #e8f0fe;"
        "  color: #1a73e8;"
        "}"
        "QHeaderView::section {"
        "  background-color: #ffffff;"
        "  color: #666666;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  border: none;"
        "  border-bottom: 2px solid #e0e0e0;"
        "  padding: 8px;"
        "}"
        );

    QHeaderView *header = ui->tableTransactions->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Stretch); // Type
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableTransactions->setColumnWidth(2, 200); // Amount
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->tableTransactions->setColumnWidth(3, 80); // ID

    ui->tableTransactions->verticalHeader()->setVisible(false);
    ui->tableTransactions->setShowGrid(false);
    ui->tableTransactions->setAlternatingRowColors(false);
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

    // connect the reply's finished signal to a lambda function to handle the response
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QJsonArray jsonArray = doc.array();

            // new page, clear old data
            ui->tableTransactions->setRowCount(0);

            if (jsonArray.size() < 10) {
                ui->btnNextPage->setEnabled(false);
            } else {
                ui->btnNextPage->setEnabled(true);
            }

            // if page is 1, disable previous page button
            ui->btnPrevPage->setEnabled(currentPage > 1);

            // populate the table with data
            for (const QJsonValue &value : jsonArray) {
                QJsonObject obj = value.toObject();
                int row = ui->tableTransactions->rowCount();
                ui->tableTransactions->insertRow(row);

                QString dateStr = obj.contains("DATE") ? obj["DATE"].toString() : obj["Aika"].toString();
                dateStr = dateStr.replace("T", " ").left(19);
                ui->tableTransactions->setItem(row, 0, new QTableWidgetItem(dateStr));

                QString type = obj["Tyyppi"].toString();
                QTableWidgetItem *typeItem = new QTableWidgetItem(type);
                typeItem->setTextAlignment(Qt::AlignCenter);
                ui->tableTransactions->setItem(row, 1, typeItem);

                // amount comes from backend as string "100.00"
                QJsonValue amtVal = obj.contains("AMOUNT") ? obj["AMOUNT"] : obj["Määrä"];
                double amount = amtVal.toString().toDouble();

                QTableWidgetItem *amountItem = new QTableWidgetItem(QString::number(amount, 'f', 2) + " €");
                amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                // green for DEPOSIT, red for others
                if (type == "DEPOSIT") {
                    amountItem->setForeground(QColor("#2e7d32"));
                } else {
                    amountItem->setForeground(QColor("#c62828"));
                }
                ui->tableTransactions->setItem(row, 2, amountItem);

                QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(obj["ID"].toInt()));
                idItem->setTextAlignment(Qt::AlignCenter);
                ui->tableTransactions->setItem(row, 3, idItem);
            }
        } else {
            qDebug() << "Error fetching data:" << reply->errorString();
            ui->btnNextPage->setEnabled(false);
        }
        reply->deleteLater();
    });
}

// next page
void TransactionHistory::on_btnNextPage_clicked()
{
    // restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }
    currentPage++;
    displayData();
}

// previous page
void TransactionHistory::on_btnPrevPage_clicked()
{
    // restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }
    if (currentPage > 1) {
        currentPage--;
        displayData();
    }
}

// back to menu
void TransactionHistory::on_btnBackToMenu_clicked()
{
    // restart timer
    if (Environment::timerLogOut) {
        Environment::timerLogOut->start();
    }
    this->close();
}
