#pragma once

#include <string>
#include <vector>



#include "transaction.h"

namespace JsonTools {



std::string addTransactionMsg(const Transaction& t);
std::string getAllMsg();

std::string okResponse(const std::string& message);
std::string errorResponse(const std::string& message);
std::string transactionsResponse(const std::vector<Transaction>& transactions);
bool isValidJson(const std::string& json);
std::string extractAction(const std::string& json);
bool extractTransaction(const std::string& json, Transaction& out);

}