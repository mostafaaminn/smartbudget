#ifndef TRANSACTION_H
#define TRANSACTION_H


#include <QString>
#include <QDate>

class Transaction {
public:
    Transaction(double amount, QString type, QString category,
                QDate date, QString currency = "USD", bool recurring = false);

    double  getAmount()    const;
    QString getType()      const;
    QString getCategory()  const;
    QDate   getDate()      const;
    QString getCurrency()  const;
    bool    isRecurring()  const;

private:
    double  amount;
    QString type;
    QString category;
    QDate   date;
    QString currency;
    bool    recurring;
};

#endif // TRANSACTION_H
