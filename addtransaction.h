#ifndef ADDTRANSACTION_H
#define ADDTRANSACTION_H

#include <QWidget>
#include "budgetmanager.h"

class History;
class statistics;

namespace Ui { class Addtransaction; }

class Addtransaction : public QWidget
{
    Q_OBJECT

public:
    explicit Addtransaction(QWidget *parent = nullptr);
    ~Addtransaction();

    // Called by child windows so the main labels stay in sync
    void refreshLabels();

private slots:
    void onAddClicked();
    void on_historyButton_clicked();
    void on_summaryButton_clicked();
    // FIX: slot is now properly wired AND actually updates the manager
    void onDisplayCurrencyChanged(const QString& text);

private:
    Ui::Addtransaction *ui;

    BudgetManager manager;
    History*      historyWindow = nullptr;
    statistics*   statsWindow   = nullptr;
};

#endif
