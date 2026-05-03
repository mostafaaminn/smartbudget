#ifndef ADDTRANSACTION_H
#define ADDTRANSACTION_H

#include <QWidget>
#include "budgetmanager.h"
#include "networkclient.h"

class History;
class statistics;

namespace Ui { class Addtransaction; }

class Addtransaction : public QWidget
{
    Q_OBJECT

signals:
    void totalsChanged(double balance, double income, double expenses,
                       int count, QString highestCategory);
    void historyRequested();
    void statisticsRequested();

public:
    explicit Addtransaction(QWidget *parent = nullptr);
    ~Addtransaction();

    BudgetManager* getManager();

    void refreshLabels();

private slots:
    void onAddClicked();
    void on_historyButton_clicked();
    void on_summaryButton_clicked();

    void onDisplayCurrencyChanged(const QString& text);

private:
    void clearInputs();
    void updateSummaryLabels();

    Ui::Addtransaction *ui;
    BudgetManager manager;
    NetworkClient *networkClient;

    History* historyWindow = nullptr;
    statistics* statsWindow = nullptr;
};

#endif
