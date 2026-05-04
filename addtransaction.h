#ifndef ADDTRANSACTION_H
#define ADDTRANSACTION_H

#include <QWidget>
#include "budgetmanager.h"
#include "networkclient.h"
#include "BudgetController.h"

// forward declarations
class History;
class statistics;

namespace Ui { class Addtransaction; }

class Addtransaction : public QWidget
{
    Q_OBJECT

public:
    explicit Addtransaction(QWidget *parent = nullptr);
    ~Addtransaction();

    BudgetManager* getManager();
    void refreshLabels();
    void updateSummaryLabels();

signals:
    void totalsChanged(double balance, double income, double expenses,
                       int count, QString highestCategory);
    void historyRequested();
    void statisticsRequested();

private slots:
    void onAddClicked();
    void onDisplayCurrencyChanged(const QString& text);

private:
    void clearInputs();

    Ui::Addtransaction* ui;
    BudgetManager       manager;
    NetworkClient*      rawClient;      // owns the socket
    BudgetController    controller;     // sends via INetworkClient interface

    History*    historyWindow = nullptr;
    statistics* statsWindow   = nullptr;
};

#endif // ADDTRANSACTION_H
