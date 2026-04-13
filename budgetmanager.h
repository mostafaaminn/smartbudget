#ifndef BUDGETMANAGER_H
#define BUDGETMANAGER_H

#include <vector>
#include <QString>
#include <QMap>
#include "transaction.h"

class BudgetManager {
private:
    std::vector<Transaction> transactions;
    QMap<QString, double> categoryBudgets;

public:
    BudgetManager();

    // core functions
    void addTransaction(const Transaction& t);
    void removeTransaction(int index);
    void updateTransaction(int index, double amount, QString type, QString category);
    void setBudget(const QString& category, double amount);
     // calculations
    double getBudget(const QString& category) const;
    double getBalance() const;
    double getTotalIncome() const;
    double getTotalExpenses() const;
    int getTransactionCount() const;
    QString getHighestSpendingCategory() const;

    std::vector<Transaction> getAllTransactions() const;
};

#endif
