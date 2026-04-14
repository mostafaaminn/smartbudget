#pragma once

#include <string>
#include <vector>
#include "transaction.h"

namespace JsonUtils {
std::string escape(const std::string& text);

std::string toJson(const Transaction& t);
bool fromJson(const std::string& json, Transaction& out);

std::string addTransactionMsg(const Transaction& t);
std::string getAllMsag();

std::string okResponse(const std::string& message);
std::string errorResponse(const std::string& message);
std::string transactionsResponse(const std::vector<Transaction>& transactions);
}