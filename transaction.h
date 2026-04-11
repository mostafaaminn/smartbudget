#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>

class Transaction {
private:
    double amount;
    QString type;
    QString category;
    QDate date;

public:
    Transaction(double amount, QString type, QString category, QDate date);

    double getAmount() const;
    QString getType() const;
    QString getCategory() const;
    QDate getDate() const;
};

#endif
