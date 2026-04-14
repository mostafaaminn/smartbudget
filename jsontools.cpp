#include "JsonTools.h"
#include <sstream>
#include <QString>

namespace JsonTools {

std::string escape(const std::string& text) {
    std::string result;
    for (char c : text) {
        if (c == '"' || c == '\\') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

std::string toJson(const Transaction& t) {
    std::ostringstream out;
    out << "{"
        << "\"amount\":" << t.getAmount() << ","
        << "\"type\":\"" << escapeJson(t.getType().toStdString()) << "\","
        << "\"category\":\"" << escapeJson(t.getCategory().toStdString()) << "\","
        << "\"date\":\"" << escapeJson(t.getDate().toString("yyyy-MM-dd").toStdString()) << "\""
        << "}";
    return out.str();
}

std::string addTransactionMsg(const Transaction& t) {
    return std::string("{\"action\":\"add_transaction\",\"transaction\":")
    + transactionToJson(t) + "}\n";
}

std::string getAllMsg() {
    return "{\"action\":\"get_all\"}\n";
}

std::string okResponse(const std::string& message) {
    return std::string("{\"status\":\"ok\",\"message\":\"") + escapeJson(message) + "\"}\n";
}

std::string errorResponse(const std::string& message) {
    return std::string("{\"status\":\"error\",\"message\":\"") + escapeJson(message) + "\"}\n";
}

std::string transactionsResponse(const std::vector<Transaction>& transactions) {
    std::ostringstream out;
    out << "{\"status\":\"ok\",\"transactions\":[";
    for (size_t i = 0; i < transactions.size(); ++i) {
        out << transactionToJson(transactions[i]);
        if (i + 1 < transactions.size()) {
            out << ",";
        }
    }
    out << "]}\n";
    return out.str();
}

static bool extractString(const std::string& json, const std::string& key, std::string& value) {
    std::string pattern = "\"" + key + "\":\"";
    size_t start = json.find(pattern);
    if (start == std::string::npos) return false;
    start += pattern.size();
    size_t end = json.find("\"", start);
    if (end == std::string::npos) return false;
    value = json.substr(start, end - start);
    return true;
}

static bool extractDouble(const std::string& json, const std::string& key, double& value) {
    std::string pattern = "\"" + key + "\":";
    size_t start = json.find(pattern);
    if (start == std::string::npos) return false;
    start += pattern.size();
    size_t end = json.find_first_of(",}", start);
    if (end == std::string::npos) return false;

    try {
        value = std::stod(json.substr(start, end - start));
        return true;
    } catch (...) {
        return false;
    }
}

bool fromJson(const std::string& json, Transaction& out) {
    double amount;
    std::string type;
    std::string category;
    std::string date;

    if (!extractDoubleField(json, "amount", amount)) return false;
    if (!extractStringField(json, "type", type)) return false;
    if (!extractStringField(json, "category", category)) return false;
    if (!extractStringField(json, "date", date)) return false;

    out = Transaction(
        amount,
        QString::fromStdString(type),
        QString::fromStdString(category),
        QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd")
        );

    return true;
}

}