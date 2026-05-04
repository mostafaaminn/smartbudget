#ifndef BUDGETCONTROLLER_H
#define BUDGETCONTROLLER_H


#include "INetworkClient.h"
#include "transaction.h"

class BudgetController
{
public:
    explicit BudgetController(INetworkClient* client);

    // Ask the server for the full transaction list.
    void requestTransactions();

    // Send a single new transaction to the server.
    void addTransactionToServer(const Transaction& transaction);

private:
    INetworkClient* networkClient;
};

#endif // BUDGETCONTROLLER_H
