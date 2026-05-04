#include "database.h"

bool Database::init()
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");


    db.setDatabaseName("smartbudget.db");

    if (!db.open()) {
        qWarning() << "Database::init – could not open:" << db.lastError().text();
        return false;
    }

    return createTables();
}

bool Database::createTables()
{
    QSqlQuery q;


    bool ok = q.exec(
        "CREATE TABLE IF NOT EXISTS transactions ("
        "  id        INTEGER PRIMARY KEY AUTOINCREMENT, "
        "  amount    REAL    NOT NULL, "
        "  type      TEXT    NOT NULL, "
        "  category  TEXT    NOT NULL, "
        "  date      TEXT    NOT NULL, "
        "  currency  TEXT    NOT NULL, "
        "  recurring INTEGER NOT NULL DEFAULT 0 "
        ");"
        );

    if (!ok) {
        qWarning() << "createTables (transactions):" << q.lastError().text();
        return false;
    }


    ok = q.exec(
        "CREATE TABLE IF NOT EXISTS budgets ("
        "  category TEXT PRIMARY KEY, "
        "  amount   REAL NOT NULL "
        ");"
        );

    if (!ok) {
        qWarning() << "createTables (budgets):" << q.lastError().text();
        return false;
    }

    return true;
}



int Database::insertTransaction(const Transaction& t)
{
    QSqlQuery q;
    q.prepare(
        "INSERT INTO transactions (amount, type, category, date, currency, recurring) "
        "VALUES (:amount, :type, :category, :date, :currency, :recurring);"
        );
    q.bindValue(":amount",    t.getAmount());
    q.bindValue(":type",      t.getType());
    q.bindValue(":category",  t.getCategory());
    q.bindValue(":date",      t.getDate().toString("yyyy-MM-dd"));
    q.bindValue(":currency",  t.getCurrency());
    q.bindValue(":recurring", t.isRecurring() ? 1 : 0);

    if (!q.exec()) {
        qWarning() << "insertTransaction:" << q.lastError().text();
        return -1;
    }


    return q.lastInsertId().toInt();
}

std::vector<Transaction> Database::loadAllTransactions()
{
    std::vector<Transaction> result;

    QSqlQuery q("SELECT amount, type, category, date, currency, recurring "
                "FROM transactions ORDER BY date ASC;");

    while (q.next()) {
        double  amount    = q.value(0).toDouble();
        QString type      = q.value(1).toString();
        QString category  = q.value(2).toString();
        QDate   date      = QDate::fromString(q.value(3).toString(), "yyyy-MM-dd");
        QString currency  = q.value(4).toString();
        bool    recurring = q.value(5).toInt() != 0;

        result.emplace_back(amount, type, category, date, currency, recurring);
    }

    return result;
}

bool Database::deleteTransaction(int dbId)
{
    QSqlQuery q;
    q.prepare("DELETE FROM transactions WHERE id = :id;");
    q.bindValue(":id", dbId);

    if (!q.exec()) {
        qWarning() << "deleteTransaction:" << q.lastError().text();
        return false;
    }
    return true;
}

bool Database::updateTransaction(int dbId, double amount,
                                 const QString& type,
                                 const QString& category,
                                 const QString& currency)
{
    QSqlQuery q;
    q.prepare(
        "UPDATE transactions "
        "SET amount = :amount, type = :type, category = :category, currency = :currency "
        "WHERE id = :id;"
        );
    q.bindValue(":amount",   amount);
    q.bindValue(":type",     type);
    q.bindValue(":category", category);
    q.bindValue(":currency", currency);
    q.bindValue(":id",       dbId);

    if (!q.exec()) {
        qWarning() << "updateTransaction:" << q.lastError().text();
        return false;
    }
    return true;
}



bool Database::saveBudget(const QString& category, double amount)
{
    QSqlQuery q;

    q.prepare(
        "INSERT OR REPLACE INTO budgets (category, amount) "
        "VALUES (:category, :amount);"
        );
    q.bindValue(":category", category);
    q.bindValue(":amount",   amount);

    if (!q.exec()) {
        qWarning() << "saveBudget:" << q.lastError().text();
        return false;
    }
    return true;
}

QMap<QString, double> Database::loadBudgets()
{
    QMap<QString, double> result;
    QSqlQuery q("SELECT category, amount FROM budgets;");

    while (q.next()) {
        result[q.value(0).toString()] = q.value(1).toDouble();
    }

    return result;
}
