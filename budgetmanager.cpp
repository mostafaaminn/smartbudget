
#include "budgetmanager.h"

#include <map>
#include <QStringList>
#include <QDate>

BudgetManager::BudgetManager()
{
}


void BudgetManager::loadFromDatabase()
{

    transactions.clear();
    dbIds.clear();

    QSqlQuery q("SELECT id, amount, type, category, date, currency, recurring "
                "FROM transactions ORDER BY date ASC;");

    while (q.next()) {
        int     id        = q.value(0).toInt();
        double  amount    = q.value(1).toDouble();
        QString type      = q.value(2).toString();
        QString category  = q.value(3).toString();
        QDate   date      = QDate::fromString(q.value(4).toString(), "yyyy-MM-dd");
        QString currency  = q.value(5).toString();
        bool    recurring = q.value(6).toInt() != 0;

        transactions.emplace_back(amount, type, category, date, currency, recurring);
        dbIds.push_back(id);
    }


    categoryBudgets = Database::loadBudgets();
}


void BudgetManager::addTransaction(const Transaction& t)
{
    int id = Database::insertTransaction(t);
    transactions.push_back(t);
    dbIds.push_back(id);
}


void BudgetManager::removeTransaction(int index)
{
    if (index < 0 || index >= static_cast<int>(transactions.size())) {
        return;
    }

    // ← NEW: remove from SQLite first
    Database::deleteTransaction(dbIds[index]);

    transactions.erase(transactions.begin() + index);
    dbIds.erase(dbIds.begin() + index);
}

void BudgetManager::updateTransaction(int index, double amount,
                                      QString type, QString category,
                                      QString currency)
{
    if (index < 0 || index >= static_cast<int>(transactions.size())) {
        return;
    }


    Database::updateTransaction(dbIds[index], amount, type, category, currency);

    transactions[index] = Transaction(amount, type, category,
                                      QDate::currentDate(), currency);

}

std::vector<Transaction> BudgetManager::getAllTransactions() const
{
    return transactions;
}

double BudgetManager::getBalance() const
{
    double balance = 0;

    for (const Transaction& t : transactions) {
        QString type   = t.getType().trimmed().toLower();
        double  amount = convertToDisplay(t.getAmount(), t.getCurrency());

        if (type == "income")        balance += amount;
        else if (type == "expense")  balance -= amount;
    }

    return balance;
}

double BudgetManager::getTotalIncome() const
{
    double total = 0;

    for (const Transaction& t : transactions) {
        if (t.getType().trimmed().toLower() == "income") {
            total += convertToDisplay(t.getAmount(), t.getCurrency());
        }
    }

    return total;
}

double BudgetManager::getTotalExpenses() const
{
    double total = 0;

    for (const Transaction& t : transactions) {
        if (t.getType().trimmed().toLower() == "expense") {
            total += convertToDisplay(t.getAmount(), t.getCurrency());
        }
    }

    return total;
}

int BudgetManager::getTransactionCount() const
{
    return static_cast<int>(transactions.size());
}

QString BudgetManager::getHighestSpendingCategory() const
{
    std::map<QString, double> categoryTotals;

    for (const Transaction& t : transactions) {
        if (t.getType().trimmed().toLower() == "expense") {
            categoryTotals[t.getCategory()] +=
                convertToDisplay(t.getAmount(), t.getCurrency());
        }
    }

    if (categoryTotals.empty()) return "None";

    double highestAmount = 0;
    for (const auto& pair : categoryTotals) {
        if (pair.second > highestAmount) highestAmount = pair.second;
    }

    QStringList highestCategories;
    for (const auto& pair : categoryTotals) {
        if (pair.second == highestAmount) highestCategories.append(pair.first);
    }

    return highestCategories.join(", ");
}


void BudgetManager::setBudget(const QString& category, double amount)
{
    categoryBudgets[category] = amount;
    Database::saveBudget(category, amount);     // ← NEW: persist to DB
}

double BudgetManager::getBudget(const QString& category) const
{
    return categoryBudgets.value(category, 0.0);
}

bool BudgetManager::isOverBudget(const QString& category) const
{
    return budgetDifference(category) < 0;
}

double BudgetManager::budgetDifference(const QString& category) const
{
    double budget = getBudget(category);
    double spent  = 0;

    for (const Transaction& t : transactions) {
        if (t.getType().trimmed().toLower() == "expense" &&
            t.getCategory() == category) {
            spent += convertToDisplay(t.getAmount(), t.getCurrency());
        }
    }

    return budget - spent;
}

double BudgetManager::convert(double amount, QString from, QString to) const
{
    const double rate = 50.0;
    from = from.trimmed().toUpper();
    to   = to.trimmed().toUpper();

    if (from == to)                      return amount;
    if (from == "USD" && to == "EGP")   return amount * rate;
    if (from == "EGP" && to == "USD")   return amount / rate;

    return amount;
}

double BudgetManager::convertToDisplay(double amount, const QString& fromCurrency) const
{
    return convert(amount, fromCurrency, displayCurrency);
}

void BudgetManager::setDisplayCurrency(const QString& currency)
{
    displayCurrency = currency.trimmed().toUpper();
}

QString BudgetManager::getDisplayCurrency() const
{
    return displayCurrency;
}

double BudgetManager::compareMonth(QDate month1, QDate month2) const
{
    double total1 = 0, total2 = 0;

    for (const Transaction& t : transactions) {
        if (t.getType().trimmed().toLower() != "expense") continue;

        double amount = convertToDisplay(t.getAmount(), t.getCurrency());

        if (t.getDate().month() == month1.month() &&
            t.getDate().year()  == month1.year())   total1 += amount;

        if (t.getDate().month() == month2.month() &&
            t.getDate().year()  == month2.year())   total2 += amount;
    }

    return total1 - total2;
}

void BudgetManager::applyRecurringTransactions(QDate currentDate)
{
    std::vector<Transaction> newTransactions;

    for (const Transaction& t : transactions) {
        if (!t.isRecurring()) continue;
        if (t.getDate().day() != currentDate.day()) continue;

        bool alreadyExistsToday = false;
        for (const Transaction& existing : transactions) {
            if (existing.getDate()     == currentDate    &&
                existing.getCategory() == t.getCategory() &&
                existing.getType()     == t.getType()     &&
                existing.getAmount()   == t.getAmount())
            {
                alreadyExistsToday = true;
                break;
            }
        }

        if (!alreadyExistsToday) {
            newTransactions.emplace_back(t.getAmount(), t.getType(),
                                         t.getCategory(), currentDate,
                                         t.getCurrency(), true);
        }
    }

    for (const Transaction& t : newTransactions) {
        addTransaction(t);
    }
}
