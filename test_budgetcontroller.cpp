#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QCoreApplication>

#include "BudgetController.h"
#include "mock_networkclient.h"
#include "transaction.h"
#include "JsonTools.h"

#include <QString>
#include <QDate>

using ::testing::Return;
using ::testing::_;
using ::testing::HasSubstr;

// ── addTransactionToServer ────────────────────────────────────────────────────

TEST(BudgetControllerTest, SendsAddMessageWhenConnected)
{
    MockNetworkClient mock;
    EXPECT_CALL(mock, isConnected()).WillOnce(Return(true));
    EXPECT_CALL(mock, sendMessage(HasSubstr("add_transaction"))).Times(1);

    BudgetController controller(&mock);
    Transaction t(100.0, "expense", "Food", QDate(2026, 4, 27), "USD");
    controller.addTransactionToServer(t);
}

TEST(BudgetControllerTest, DoesNotSendWhenDisconnected)
{
    MockNetworkClient mock;
    EXPECT_CALL(mock, isConnected()).WillOnce(Return(false));
    EXPECT_CALL(mock, sendMessage(_)).Times(0);

    BudgetController controller(&mock);
    Transaction t(100.0, "expense", "Food", QDate(2026, 4, 27), "USD");
    controller.addTransactionToServer(t);
}

TEST(BudgetControllerTest, SentMessageContainsTransactionField)
{
    MockNetworkClient mock;
    EXPECT_CALL(mock, isConnected()).WillOnce(Return(true));
    EXPECT_CALL(mock, sendMessage(HasSubstr("\"transaction\""))).Times(1);

    BudgetController controller(&mock);
    Transaction t(50.0, "income", "salary", QDate(2026, 4, 27), "USD");
    controller.addTransactionToServer(t);
}

TEST(BudgetControllerTest, SentMessageEndsWithNewline)
{
    MockNetworkClient mock;
    std::string captured;

    EXPECT_CALL(mock, isConnected()).WillOnce(Return(true));
    EXPECT_CALL(mock, sendMessage(_))
        .WillOnce([&](const std::string& msg) { captured = msg; });

    BudgetController controller(&mock);
    Transaction t(25.0, "expense", "Transport", QDate(2026, 4, 27), "USD");
    controller.addTransactionToServer(t);

    ASSERT_FALSE(captured.empty());
    EXPECT_EQ(captured.back(), '\n');
}

// ── requestTransactions ───────────────────────────────────────────────────────

TEST(BudgetControllerTest, RequestTransactionsSendsGetAllWhenConnected)
{
    MockNetworkClient mock;
    EXPECT_CALL(mock, isConnected()).WillOnce(Return(true));
    EXPECT_CALL(mock, sendMessage(HasSubstr("get_all"))).Times(1);

    BudgetController controller(&mock);
    controller.requestTransactions();
}

TEST(BudgetControllerTest, RequestTransactionsDoesNotSendWhenDisconnected)
{
    MockNetworkClient mock;
    EXPECT_CALL(mock, isConnected()).WillOnce(Return(false));
    EXPECT_CALL(mock, sendMessage(_)).Times(0);

    BudgetController controller(&mock);
    controller.requestTransactions();
}

