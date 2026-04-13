#include "budgetManager.h"
#include <map>
#include <QStringList>
#include <QDate>

BudgetManager::BudgetManager() {
    // nothing needed for now (vector starts empty automatically)
}

void BudgetManager::addTransaction(const Transaction& t) {
    transactions.push_back(t);
}

void BudgetManager::removeTransaction(int index) {
    if (index >= 0 && index < transactions.size()) {
        transactions.erase(transactions.begin() + index);
    }
}

double BudgetManager::getBalance() const {
    double balance = 0;

    for (const Transaction& t : transactions) {
        if (t.getType() == "income") {
            balance += t.getAmount();
        } else if (t.getType() == "expense") {
            balance -= t.getAmount();
        }
    }

    return balance;
}

double BudgetManager::getTotalIncome() const {
    double total = 0;

    for (const Transaction& t : transactions) {
        if (t.getType() == "income") {
            total += t.getAmount();
        }
    }

    return total;
}

double BudgetManager::getTotalExpenses() const {
    double total = 0;

    for (const Transaction& t : transactions) {
        if (t.getType() == "expense") {
            total += t.getAmount();
        }
    }

    return total;
}

std::vector<Transaction> BudgetManager::getAllTransactions() const {
    return transactions;
}

int BudgetManager::getTransactionCount() const
{
    return transactions.size();
}

QString BudgetManager::getHighestSpendingCategory() const
{
    std::map<QString, double> categoryTotals;

    for (const Transaction& t : transactions) {
        if (t.getType() == "expense") {
            categoryTotals[t.getCategory()] += t.getAmount();
        }
    }

    if (categoryTotals.empty()) {
        return "None";
    }

    double highestAmount = 0;

    for (const auto& pair : categoryTotals) {
        if (pair.second > highestAmount) {
            highestAmount = pair.second;
        }
    }

    QStringList highestCategories;

    for (const auto& pair : categoryTotals) {
        if (pair.second == highestAmount) {
            highestCategories.append(pair.first);
        }
    }

    return highestCategories.join(", ");
}

void BudgetManager::updateTransaction(int index, double amount, QString type, QString category)
{
    if (index < 0 || index >= transactions.size()) {
        return;
    }

    transactions[index] = Transaction(amount, type, category, QDate::currentDate());
}

void BudgetManager::setBudget(const QString& category, double amount)
{
    categoryBudgets[category] = amount;
}

double BudgetManager::getBudget(const QString& category) const
{
    return categoryBudgets.value(category, 0);
}
