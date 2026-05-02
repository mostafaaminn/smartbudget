#include "jsontools.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QByteArray>
#include <QString>
#include <QDate>

namespace JsonTools {

static QJsonObject transactionToObject(const Transaction& t)
{
    QJsonObject obj;

    obj["amount"] = t.getAmount();
    obj["type"] = t.getType();
    obj["category"] = t.getCategory();
    obj["date"] = t.getDate().toString("yyyy-MM-dd");

    return obj;
}

static bool objectToTransaction(const QJsonObject& obj, Transaction& out)
{
    if (!obj.contains("amount") || !obj["amount"].isDouble()) {
        return false;
    }

    if (!obj.contains("type") || !obj["type"].isString()) {
        return false;
    }

    if (!obj.contains("category") || !obj["category"].isString()) {
        return false;
    }

    if (!obj.contains("date") || !obj["date"].isString()) {
        return false;
    }

    double amount = obj["amount"].toDouble();
    QString type = obj["type"].toString();
    QString category = obj["category"].toString();
    QDate date = QDate::fromString(obj["date"].toString(), "yyyy-MM-dd");

    if (!date.isValid()) {
        return false;
    }

    out = Transaction(amount, type, category, date, "EGP");
    return true;
}

std::string toJson(const Transaction& t)
{
    QJsonObject obj = transactionToObject(t);
    QJsonDocument doc(obj);

    return doc.toJson(QJsonDocument::Compact).toStdString();
}

bool fromJson(const std::string& json, Transaction& out)
{
    QByteArray data = QByteArray::fromStdString(json);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    if (!doc.isObject()) {
        return false;
    }

    return objectToTransaction(doc.object(), out);
}

std::string addTransactionMsg(const Transaction& t)
{
    QJsonObject root;

    root["action"] = "add_transaction";
    root["transaction"] = transactionToObject(t);

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Compact).toStdString() + "\n";
}

std::string getAllMsg()
{
    QJsonObject root;

    root["action"] = "get_all";

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Compact).toStdString() + "\n";
}

std::string okResponse(const std::string& message)
{
    QJsonObject root;

    root["status"] = "ok";
    root["message"] = QString::fromStdString(message);

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Compact).toStdString() + "\n";
}

std::string errorResponse(const std::string& message)
{
    QJsonObject root;

    root["status"] = "error";
    root["message"] = QString::fromStdString(message);

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Compact).toStdString() + "\n";
}

std::string transactionsResponse(const std::vector<Transaction>& transactions)
{
    QJsonObject root;

    root["status"] = "ok";

    QJsonArray array;

    for (const Transaction& t : transactions) {
        array.append(transactionToObject(t));
    }

    root["transactions"] = array;

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Compact).toStdString() + "\n";
}

bool isValidJson(const std::string& json)
{
    QByteArray data = QByteArray::fromStdString(json);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    return error.error == QJsonParseError::NoError && !doc.isNull();
}

std::string extractAction(const std::string& json)
{
    QByteArray data = QByteArray::fromStdString(json);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return "";
    }

    if (!doc.isObject()) {
        return "";
    }

    QJsonObject root = doc.object();

    if (!root.contains("action") || !root["action"].isString()) {
        return "";
    }

    return root["action"].toString().toStdString();
}

bool extractTransaction(const std::string& json, Transaction& out)
{
    QByteArray data = QByteArray::fromStdString(json);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    if (!doc.isObject()) {
        return false;
    }

    QJsonObject root = doc.object();

    if (!root.contains("transaction") || !root["transaction"].isObject()) {
        return false;
    }

    QJsonObject transactionObj = root["transaction"].toObject();

    return objectToTransaction(transactionObj, out);
}

}
