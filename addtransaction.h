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

    // Called by child windows so the main labels stay in sync
    void refreshLabels();

private slots:
    void onAddClicked();
    void on_historyButton_clicked();
    void on_summaryButton_clicked();

    // Updates the displayed currency and refreshes the manager labels
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
