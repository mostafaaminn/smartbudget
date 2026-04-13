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
    void onDeleteClicked();

signals:
    void totalsChanged(double balance, double income, double expenses,
                       int count, QString highestCategory);

public:
    explicit Addtransaction(QWidget *parent = nullptr);
    ~Addtransaction();

private:
    void clearInputs();
    void updateSummaryLabels();
    void updateTransactionList();
    QString buildTransactionJson(double amount, const QString& type, const QString& category, const QDate& date) const;

    Ui::Addtransaction *ui;
    BudgetManager manager;
    NetworkClient *networkClient;
};

#endif // ADDTRANSACTION_H
