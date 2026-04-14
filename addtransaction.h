#ifndef ADDTRANSACTION_H
#define ADDTRANSACTION_H

#include <QWidget>
#include "budgetmanager.h"
#include "networkclient.h"

namespace Ui {
class Addtransaction;
}

class Addtransaction : public QWidget
{
    Q_OBJECT

private slots:
    void onAddClicked();

signals:
    void totalsChanged(double balance, double income, double expenses,
                       int count, QString highestCategory);
    void historyRequested();
    void statisticsRequested();

public:
    explicit Addtransaction(QWidget *parent = nullptr);
    ~Addtransaction();

    BudgetManager* getManager();

private:
    void clearInputs();
    void updateSummaryLabels();

    Ui::Addtransaction *ui;
    BudgetManager manager;
    NetworkClient *networkClient;
};

#endif
