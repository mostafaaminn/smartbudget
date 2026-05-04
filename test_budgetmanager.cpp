#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "budgetmanager.h"
#include "transaction.h"
#include "database.h"

class BudgetManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        QSqlQuery q;
        q.exec("DELETE FROM transactions;");
        q.exec("DELETE FROM budgets;");
        q.exec("DELETE FROM sqlite_sequence WHERE name='transactions';");
    }

    BudgetManager manager;
};

TEST_F(BudgetManagerTest, StartsEmpty)
{
    EXPECT_DOUBLE_EQ(manager.getBalance(), 0.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 0.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 0.0);
    EXPECT_EQ(manager.getTransactionCount(), 0);
    EXPECT_EQ(manager.getHighestSpendingCategory(), "None");
}

TEST_F(BudgetManagerTest, AddIncomeTransactionUpdatesBalanceAndIncome)
{
    Transaction t(500.0, "income", "salary", QDate(2026, 4, 12), "USD");
    manager.addTransaction(t);

    EXPECT_DOUBLE_EQ(manager.getBalance(), 500.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 500.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 0.0);
    EXPECT_EQ(manager.getTransactionCount(), 1);
}

TEST_F(BudgetManagerTest, AddExpenseTransactionUpdatesBalanceAndExpenses)
{
    Transaction t(200.0, "expense", "food", QDate(2026, 4, 12), "USD");
    manager.addTransaction(t);

    EXPECT_DOUBLE_EQ(manager.getBalance(), -200.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 0.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 200.0);
    EXPECT_EQ(manager.getTransactionCount(), 1);
}

TEST_F(BudgetManagerTest, MultipleTransactionsCalculateCorrectTotals)
{
    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(200.0, "expense", "food", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(150.0, "expense", "transport", QDate(2026, 4, 12), "USD"));

    EXPECT_DOUBLE_EQ(manager.getBalance(), 650.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 1000.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 350.0);
    EXPECT_EQ(manager.getTransactionCount(), 3);
}

TEST_F(BudgetManagerTest, RemoveTransactionDeletesCorrectItem)
{
    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(300.0, "expense", "food", QDate(2026, 4, 12), "USD"));

    manager.removeTransaction(1);

    EXPECT_DOUBLE_EQ(manager.getBalance(), 1000.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 1000.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 0.0);
    EXPECT_EQ(manager.getTransactionCount(), 1);
}

TEST_F(BudgetManagerTest, RemoveInvalidIndexDoesNothing)
{
    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));

    manager.removeTransaction(-1);
    manager.removeTransaction(5);

    EXPECT_DOUBLE_EQ(manager.getBalance(), 1000.0);
    EXPECT_EQ(manager.getTransactionCount(), 1);
}

TEST_F(BudgetManagerTest, HighestSpendingCategoryReturnsCorrectCategory)
{
    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(200.0, "expense", "food", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(300.0, "expense", "transport", QDate(2026, 4, 12), "USD"));

    EXPECT_EQ(manager.getHighestSpendingCategory(), "transport");
}

TEST_F(BudgetManagerTest, HighestSpendingCategoryIgnoresIncome)
{
    manager.addTransaction(Transaction(5000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(100.0, "expense", "food", QDate(2026, 4, 12), "USD"));

    EXPECT_EQ(manager.getHighestSpendingCategory(), "food");
}

TEST_F(BudgetManagerTest, HighestSpendingCategoryReturnsAllTiedCategories)
{
    manager.addTransaction(Transaction(200.0, "expense", "food", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(200.0, "expense", "transport", QDate(2026, 4, 12), "USD"));

    QString result = manager.getHighestSpendingCategory();
    EXPECT_TRUE(result == "food, transport" || result == "transport, food");
}

TEST_F(BudgetManagerTest, HighestSpendingCategoryReturnsNoneWhenNoExpenses)
{
    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    EXPECT_EQ(manager.getHighestSpendingCategory(), "None");
}

TEST_F(BudgetManagerTest, UpdateTransactionChangesStoredValues)
{
    manager.addTransaction(Transaction(100.0, "expense", "Food", QDate(2026, 4, 12), "USD"));
    manager.updateTransaction(0, 250.0, "income", "Salary", "USD");

    auto transactions = manager.getAllTransactions();

    ASSERT_EQ(transactions.size(), 1);
    EXPECT_DOUBLE_EQ(transactions[0].getAmount(), 250.0);
    EXPECT_EQ(transactions[0].getType(), "income");
    EXPECT_EQ(transactions[0].getCategory(), "Salary");
}

TEST_F(BudgetManagerTest, SetAndGetBudgetWorksCorrectly)
{
    manager.setBudget("Food", 1000.0);

    EXPECT_DOUBLE_EQ(manager.getBudget("Food"), 1000.0);
    EXPECT_DOUBLE_EQ(manager.getBudget("Rent"), 0.0);
}

TEST_F(BudgetManagerTest, RemoveFirstTransactionUpdatesTotalsCorrectly)
{
    manager.addTransaction(Transaction(1000.0, "income", "salary", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(200.0, "expense", "food", QDate(2026, 4, 12), "USD"));
    manager.addTransaction(Transaction(300.0, "expense", "transport", QDate(2026, 4, 12), "USD"));

    manager.removeTransaction(0);

    EXPECT_DOUBLE_EQ(manager.getBalance(), -500.0);
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 0.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpenses(), 500.0);
    EXPECT_EQ(manager.getTransactionCount(), 2);
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    db.open();
    Database::init();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
