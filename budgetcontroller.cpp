#include "BudgetController.h"
#include "JsonTools.h"

BudgetController::BudgetController(INetworkClient* networkClient)
    : networkClient(networkClient)
{
}

void BudgetController::requestTransactions()
{
    if (networkClient && networkClient->isConnected()) {
        networkClient->sendMessage(JsonTools::getAllMsg());
    }
}

void BudgetController::addTransactionToServer(const Transaction& transaction)
{
    if (networkClient && networkClient->isConnected()) {
        networkClient->sendMessage(JsonTools::addTransactionMsg(transaction));
    }
}