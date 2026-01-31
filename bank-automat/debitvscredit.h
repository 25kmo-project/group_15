#ifndef DEBITVSCREDIT_H
#define DEBITVSCREDIT_H

#include <QDialog>
#include <qdebug.h>

namespace Ui {
class DebitvsCredit;
}

class DebitvsCredit : public QDialog
{
    Q_OBJECT

public:
    explicit DebitvsCredit(QWidget *parent = nullptr);
    ~DebitvsCredit();

    void setToken(const QString &newToken);

    QString getToken() const;

private:
    Ui::DebitvsCredit *ui;
    QString token;
};

#endif // DEBITVSCREDIT_H
