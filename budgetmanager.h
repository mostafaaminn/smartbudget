#ifndef BUDGETMANAGER_H
#define BUDGETMANAGER_H

#include "database.h"
#include <vector>
#include <QString>
#include <QMap>
#include <QDate>
#include "transaction.h"

struct SavingsGoal {
    QString name;
    double  targetAmount;
    double  savedAmount;
};

struct BillReminder {
    QString name;
    double  amount;
    int     dayOfMonth;
    QString currency;
};

class BudgetManager {
public:
    BudgetManager();

    void loadFromDatabase();

    void addTransaction(const Transaction& t);
    void removeTransaction(int index);
    void updateTransaction(int index, double amount, QString type,
                           QString category, QString currency);

    std::vector<Transaction> getAllTransactions() const;

    double  getBalance()                  const;
    double  getTotalIncome()              const;
    double  getTotalExpenses()            const;
    int     getTransactionCount()         const;
    QString getHighestSpendingCategory()  const;

    // Budget
    void   setBudget(const QString& category, double amount);
    double getBudget(const QString& category) const;
    bool   isOverBudget(const QString& category) const;
    double budgetDifference(const QString& category) const;
    // Returns categories that have exceeded their budget
    QStringList getExceededBudgetCategories() const;
    // Returns categories approaching budget (>80% spent)
    QStringList getApproachingBudgetCategories() const;

    // Month comparison
    double compareMonth(QDate month1, QDate month2) const;
    // Returns a human-readable summary of month comparison
    QString compareMonthSummary(QDate month1, QDate month2) const;

    // Display currency
    void    setDisplayCurrency(const QString& currency);
    QString getDisplayCurrency() const;

    // Recurring transactions
    void applyRecurringTransactions(QDate currentDate);

    // Savings goals
    void addSavingsGoal(const SavingsGoal& goal);
    void removeSavingsGoal(int index);
    void contributeSavingsGoal(int index, double amount);
    const std::vector<SavingsGoal>& getSavingsGoals() const;

    // Bill reminders
    void addBillReminder(const BillReminder& bill);
    void removeBillReminder(int index);
    const std::vector<BillReminder>& getBillReminders() const;
    // Returns reminders due within the next N days
    QStringList getUpcomingBills(QDate today, int withinDays = 7) const;

    // Currency helpers
    double convert(double amount, QString from, QString to) const;
    double convertToDisplay(double amount, const QString& fromCurrency) const;

private:
    std::vector<Transaction>  transactions;
    std::vector<int>          dbIds;
    QMap<QString, double>     categoryBudgets;
    QString                   displayCurrency = "USD";
    std::vector<SavingsGoal>  savingsGoals;
    std::vector<BillReminder> billReminders;
};

#endif
