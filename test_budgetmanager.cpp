#include <gtest/gtest.h>

#include "budgetmanager.h"
#include "transaction.h"

TEST(BudgetManagerTest, StartsEmpty)
{
    BudgetManager manager;

    EXPECT_DOUBLE_EQ(manager.getBalance(), 0.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 0.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 0.0);
    EXPECT_EQ(manager.getTransactionCount(), 0);
    EXPECT_EQ(manager.getHighestSpendingCategory(), "None");
}

TEST(BudgetManagerTest, AddIncomeTransactionUpdatesBalanceAndIncome)
{
    BudgetManager manager;
    Transaction t(500.0, "income", "salary", QDate(2026, 4, 12), "USD");

    manager.addTransaction(t);

    EXPECT_DOUBLE_EQ(manager.getBalance(), 500.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 500.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 0.0);
    EXPECT_EQ(manager.getTransactionCount(), 1);
}

TEST(BudgetManagerTest, AddExpenseTransactionUpdatesBalanceAndExpenses)
{
    BudgetManager manager;
    Transaction t(200.0, "expense", "food", QDate(2026, 4, 12), "USD");

    manager.addTransaction(t);

    EXPECT_DOUBLE_EQ(manager.getBalance(), -200.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 0.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 200.0);
    EXPECT_EQ(manager.getTransactionCount(), 1);
}

TEST(BudgetManagerTest, MultipleTransactionsCalculateCorrectTotals)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(200.0, "expense", "food", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(150.0, "expense", "transport", QDate(2026, 4, 12), "USD"));

    EXPECT_DOUBLE_EQ(manager.getBalance(), 650.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 1000.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 350.0);
    EXPECT_EQ(manager.getTransactionCount(), 3);
}

TEST(BudgetManagerTest, RemoveTransactionDeletesCorrectItem)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(300.0, "expense", "food", QDate(2026, 4, 12), "USD"));

    manager.removeTransaction(1);

    EXPECT_DOUBLE_EQ(manager.getBalance(), 1000.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 1000.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 0.0);
    EXPECT_EQ(manager.getTransactionCount(), 1);
}

TEST(BudgetManagerTest, RemoveInvalidIndexDoesNothing)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));

    manager.removeTransaction(-1);
    manager.removeTransaction(5);

    EXPECT_DOUBLE_EQ(manager.getBalance(), 1000.0);
    EXPECT_EQ(manager.getTransactionCount(), 1);
}

TEST(BudgetManagerTest, HighestSpendingCategoryReturnsCorrectCategory)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(200.0, "expense", "food", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(300.0, "expense", "transport", QDate(2026, 4, 12), "USD"));

    EXPECT_EQ(manager.getHighestSpendingCategory(), "transport");
}

TEST(BudgetManagerTest, HighestSpendingCategoryIgnoresIncome)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(5000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(100.0, "expense", "food", QDate(2026, 4, 12), "USD"));

    EXPECT_EQ(manager.getHighestSpendingCategory(), "food");
}

TEST(BudgetManagerTest, HighestSpendingCategoryReturnsAllTiedCategories)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(200.0, "expense", "food", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(200.0, "expense", "transport", QDate(2026, 4, 12), "USD"));

    QString result = manager.getHighestSpendingCategory();

    EXPECT_TRUE(result == "food, transport" || result == "transport, food");
}

TEST(BudgetManagerTest, HighestSpendingCategoryReturnsNoneWhenNoExpenses)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));

    EXPECT_EQ(manager.getHighestSpendingCategory(), "None");
}

TEST(BudgetManagerTest, UpdateTransactionChangesStoredValues)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(100.0, "expense", "Food", QDate(2026, 4, 12), "USD"));
    manager.updateTransaction(0, 250.0, "income", "Salary", "USD");

    auto transactions = manager.getAllTransactions();

    ASSERT_EQ(transactions.size(), 1);
    EXPECT_DOUBLE_EQ(transactions[0].getAmount(), 250.0);
    EXPECT_EQ(transactions[0].getType(), "income");
    EXPECT_EQ(transactions[0].getCategory(), "Salary");
}

TEST(BudgetManagerTest, SetAndGetBudgetWorksCorrectly)
{
    BudgetManager manager;

    manager.setBudget("Food", 1000.0);

    EXPECT_DOUBLE_EQ(manager.getBudget("Food"), 1000.0);
    EXPECT_DOUBLE_EQ(manager.getBudget("Rent"), 0.0);
}

TEST(BudgetManagerTest, RemoveFirstTransactionUpdatesTotalsCorrectly)
{
    BudgetManager manager;

    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(200.0, "expense", "food", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(300.0, "expense", "transport", QDate(2026, 4, 12), "USD"));

    manager.removeTransaction(0);

    EXPECT_DOUBLE_EQ(manager.getBalance(), -500.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 0.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 500.0);
    EXPECT_EQ(manager.getTransactionCount(), 2);
}
