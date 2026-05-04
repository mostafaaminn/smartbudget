#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <QCloseEvent>
#include <QDate>
#include <vector>

#include "transaction.h"
#include "budgetmanager.h"

class Addtransaction;

namespace Ui {
class History;
}

class History : public QWidget
{
    Q_OBJECT

signals:
    void windowClosed();

public:
    explicit History(QWidget *parent = nullptr);
    ~History();

    void setManager(BudgetManager* m);
    void setMainWindow(Addtransaction* w);
    void setTransactions(const std::vector<Transaction>& transactions);
    void refreshTable();

    // Filter controls — call these then refreshTable() is called automatically
    void applyDateFilter(const QDate& start, const QDate& end);
    void applyAmountFilter(double minAmount, double maxAmount);
    void clearFilters();

    // Shows a dialog so the user can pick a filter interactively
    void showFilterDialog();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_deleteButton_clicked();
    void on_editButton_clicked();

private:
    Ui::History*    ui;
    BudgetManager*  manager    = nullptr;
    Addtransaction* mainWindow = nullptr;

    // Active filters (-1 / invalid means "not set")
    QDate  filterStartDate;
    QDate  filterEndDate;
    double filterMinAmount = -1;
    double filterMaxAmount = -1;
};

#endif // HISTORY_H
