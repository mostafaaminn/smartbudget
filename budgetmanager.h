#ifndef BUDGETMANAGER_H
#define BUDGETMANAGER_H

#include <vector>
#include <QString>
#include <QMap>
#include "transaction.h"

class BudgetManager {
public:
    BudgetManager();

    void addTransaction(const Transaction& t);
    void removeTransaction(int index);

    void updateTransaction(int index, double amount, QString type,
                           QString category, QString currency);

    std::vector<Transaction> getAllTransactions() const;

    double getBalance() const;
    double getTotalIncome() const;
    double getTotalExpenses() const;
    int getTransactionCount() const;
    QString getHighestSpendingCategory() const;

    void setBudget(const QString& category, double amount);
    double getBudget(const QString& category) const;
    bool isOverBudget(const QString& category) const;
    double budgetDifference(const QString& category) const;
    double compareMonth(QDate month1, QDate month2) const;
    void setDisplayCurrency(const QString& currency);
    QString getDisplayCurrency() const;
    void applyRecurringTransactions(QDate currentDate);
    double convert(double amount, QString from, QString to) const;
    double convertToDisplay(double amount, const QString& fromCurrency) const;
    std::vector<Transaction> transactions;
private:

    QMap<QString, double> categoryBudgets;
    QString displayCurrency = "USD";
};

#endif
