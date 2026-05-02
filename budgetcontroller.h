#ifndef BUDGETCONTROLLER_H
#define BUDGETCONTROLLER_H

#include "INetworkClient.h"
#include "transaction.h"

class BudgetController
{
public:
    explicit BudgetController(INetworkClient* client);

    void requestTransactions();
    void addTransactionToServer(const Transaction& transaction);

private:
    INetworkClient* networkClient;
};

#endif