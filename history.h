#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <QCloseEvent>
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

private slots:
    void on_deleteButton_clicked();
    void on_backButton_clicked();
    void on_editButton_clicked();

public:
    explicit History(QWidget *parent = nullptr);
    void setManager(BudgetManager* m);
    void setMainWindow(Addtransaction* w);
    void setTransactions(const std::vector<Transaction>& transactions);
    ~History();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::History *ui;
    BudgetManager* manager = nullptr;
    void refreshTable();
    Addtransaction* mainWindow = nullptr;
};

#endif // HISTORY_H
