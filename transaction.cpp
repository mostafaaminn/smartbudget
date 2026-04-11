#include "transaction.h"

Transaction::Transaction(double amount, QString type, QString category, QDate date)
    : amount(amount), type(type), category(category), date(date) {}

double Transaction::getAmount() const {
    return amount;
}

QString Transaction::getType() const {
    return type;
}

QString Transaction::getCategory() const {
    return category;
}

QDate Transaction::getDate() const {
    return date;
}
