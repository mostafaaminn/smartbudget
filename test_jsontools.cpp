#include <gtest/gtest.h>
#include "JsonTools.h"
#include "transaction.h"
#include <QString>
#include <QDate>
#include <string>

TEST(JsonToolsTest, ConvertTransactionToJson)
{
    Transaction t(100.5, "income", "salary", QDate(2026, 4, 27));
    std::string json = JsonTools::toJson(t);

    EXPECT_TRUE(JsonTools::isValidJson(json));
    EXPECT_NE(json.find("\"amount\":100.5"),           std::string::npos);
    EXPECT_NE(json.find("\"type\":\"income\""),         std::string::npos);
    EXPECT_NE(json.find("\"category\":\"salary\""),     std::string::npos);
    EXPECT_NE(json.find("\"date\":\"2026-04-27\""),     std::string::npos);
}

TEST(JsonToolsTest, ParseTransactionFromJson)
{
    std::string json =
        "{\"amount\":75.25,\"type\":\"expense\","
        "\"category\":\"food\",\"date\":\"2026-04-27\"}";

    Transaction t(0.0, "", "", QDate());
    bool result = JsonTools::fromJson(json, t);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(t.getAmount(),   75.25);
    EXPECT_EQ(t.getType(),            "expense");
    EXPECT_EQ(t.getCategory(),        "food");
    EXPECT_EQ(t.getDate(),            QDate(2026, 4, 27));
}

TEST(JsonToolsTest, RejectInvalidTransactionJson)
{
    std::string json =
        "{\"amount\":\"wrong\",\"type\":\"expense\","
        "\"category\":\"food\",\"date\":\"2026-04-27\"}";

    Transaction t(0.0, "", "", QDate());
    EXPECT_FALSE(JsonTools::fromJson(json, t));
}

TEST(JsonToolsTest, CreateAddTransactionMessage)
{
    Transaction t(50.0, "expense", "transport", QDate(2026, 4, 27));
    std::string msg = JsonTools::addTransactionMsg(t);

    std::string cleanMsg = msg;
    if (!cleanMsg.empty() && cleanMsg.back() == '\n')
        cleanMsg.pop_back();

    EXPECT_TRUE(JsonTools::isValidJson(cleanMsg));
    EXPECT_NE(msg.find("\"action\":\"add_transaction\""), std::string::npos);
    EXPECT_NE(msg.find("\"transaction\""),                std::string::npos);
    EXPECT_EQ(msg.back(), '\n');
}

TEST(JsonToolsTest, CreateGetAllMessage)
{
    std::string msg = JsonTools::getAllMsg();

    std::string cleanMsg = msg;
    if (!cleanMsg.empty() && cleanMsg.back() == '\n')
        cleanMsg.pop_back();

    EXPECT_TRUE(JsonTools::isValidJson(cleanMsg));
    EXPECT_NE(msg.find("\"action\":\"get_all\""), std::string::npos);
    EXPECT_EQ(msg.back(), '\n');
}

TEST(JsonToolsTest, CreateOkResponse)
{
    std::string msg = JsonTools::okResponse("Done");

    std::string cleanMsg = msg;
    if (!cleanMsg.empty() && cleanMsg.back() == '\n')
        cleanMsg.pop_back();

    EXPECT_TRUE(JsonTools::isValidJson(cleanMsg));
    EXPECT_NE(msg.find("\"status\":\"ok\""),      std::string::npos);
    EXPECT_NE(msg.find("\"message\":\"Done\""),   std::string::npos);
}

TEST(JsonToolsTest, CreateErrorResponse)
{
    std::string msg = JsonTools::errorResponse("Failed");

    std::string cleanMsg = msg;
    if (!cleanMsg.empty() && cleanMsg.back() == '\n')
        cleanMsg.pop_back();

    EXPECT_TRUE(JsonTools::isValidJson(cleanMsg));
    EXPECT_NE(msg.find("\"status\":\"error\""),     std::string::npos);
    EXPECT_NE(msg.find("\"message\":\"Failed\""),   std::string::npos);
}

TEST(JsonToolsTest, ExtractAction)
{
    std::string msg = "{\"action\":\"get_all\"}";
    EXPECT_EQ(JsonTools::extractAction(msg), "get_all");
}

TEST(JsonToolsTest, ExtractTransaction)
{
    std::string msg =
        "{\"action\":\"add_transaction\","
        "\"transaction\":{\"amount\":20,\"type\":\"expense\","
        "\"category\":\"food\",\"date\":\"2026-04-27\"}}";

    Transaction t(0.0, "", "", QDate());
    EXPECT_TRUE(JsonTools::extractTransaction(msg, t));
    EXPECT_DOUBLE_EQ(t.getAmount(),   20.0);
    EXPECT_EQ(t.getType(),            "expense");
    EXPECT_EQ(t.getCategory(),        "food");
    EXPECT_EQ(t.getDate(),            QDate(2026, 4, 27));
}

TEST(JsonToolsTest, RejectCorruptedJson)
{
    std::string msg = "{\"action\":\"get_all\"";
    EXPECT_FALSE(JsonTools::isValidJson(msg));
}

