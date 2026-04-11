#ifndef BUDGETMANAGER_H
#define BUDGETMANAGER_H

#include <vector>
#include "Transaction.h"

class BudgetManager {
private:
    std::vector<Transaction> transactions;

public:
    BudgetManager();

    // core functions
    void addTransaction(const Transaction& t);
    void removeTransaction(int index);

    // calculations
    double getBalance() const;
    double getTotalIncome() const;
    double getTotalExpenses() const;


    std::vector<Transaction> getAllTransactions() const;
};

#endif