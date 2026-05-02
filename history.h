#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <QWidget>
#include "transaction.h"
#include "budgetmanager.h"

class Addtransaction;

namespace Ui { class History; }

class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);
    ~History();

    void setManager(BudgetManager* m);
    void setMainWindow(Addtransaction* w);
    void refreshTable();

private slots:
    void on_deleteButton_clicked();
    void on_backButton_clicked();
    void on_editButton_clicked();

private:
    Ui::History*   ui;
    BudgetManager* manager    = nullptr;  // FIX: initialize to nullptr
    Addtransaction* mainWindow = nullptr;
};

#endif
