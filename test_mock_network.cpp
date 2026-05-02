#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mock_networkclient.h"
#include "BudgetController.h"

using ::testing::Return;
using ::testing::_;
using ::testing::HasSubstr;

TEST(MockNetworkClientTest, SendsMessageWhenConnected)
{
    MockNetworkClient mockClient;
    BudgetController controller(&mockClient);

    EXPECT_CALL(mockClient, isConnected())
        .WillOnce(Return(true));

    EXPECT_CALL(mockClient, sendMessage(HasSubstr("\"action\":\"get_all\"")))
        .Times(1);

    controller.requestTransactions();
}

TEST(MockNetworkClientTest, DoesNotSendWhenDisconnected)
{
    MockNetworkClient mockClient;
    BudgetController controller(&mockClient);

    EXPECT_CALL(mockClient, isConnected())
        .WillOnce(Return(false));

    EXPECT_CALL(mockClient, sendMessage(_))
        .Times(0);

    controller.requestTransactions();
}