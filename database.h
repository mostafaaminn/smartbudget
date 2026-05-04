#ifndef DATABASE_H
#define DATABASE_H

// ─── NEW FILE ────────────────────────────────────────────────────────────────
// database.h
// Wraps QSqlDatabase (SQLite) and exposes simple CRUD methods used by
// BudgetManager.  Every transaction and every category-budget is persisted
// here so data survives between application restarts.
// ─────────────────────────────────────────────────────────────────────────────

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QString>
#include <QDate>
#include <vector>
#include <QMap>

#include "transaction.h"

class Database
{
public:

    static bool init();




    static int  insertTransaction(const Transaction& t);


    static std::vector<Transaction> loadAllTransactions();


    static bool deleteTransaction(int dbId);


    static bool updateTransaction(int dbId, double amount,
                                  const QString& type,
                                  const QString& category,
                                  const QString& currency);


    static bool saveBudget(const QString& category, double amount);


    static QMap<QString, double> loadBudgets();

private:

    static bool createTables();
};

#endif
