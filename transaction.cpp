#include "transaction.h"

Transaction::Transaction(double amount, QString type, QString category, QDate date, QString currency)
    : amount(amount), type(type), category(category), date(date), currency(currency) {}

double Transaction::getAmount()   const { return amount;   }
QString Transaction::getType()    const { return type;     }
QString Transaction::getCategory()const { return category; }
QDate Transaction::getDate()      const { return date;     }
QString Transaction::getCurrency()const { return currency; }
