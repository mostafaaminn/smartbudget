#ifndef STATISTICS_H
#define STATISTICS_H
#include <QString>
#include <QMap>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include "budgetmanager.h"

class Addtransaction;

namespace Ui { class statistics; }

class statistics : public QWidget
{
    Q_OBJECT

public:
    explicit statistics(QWidget *parent = nullptr);
    ~statistics();

    void setManager(BudgetManager* m);
    void setMainWindow(Addtransaction* w);
    void updateStats();

private slots:
    void on_back1Button_clicked();
    // FIX: removed updateDisplayCurrency() — it was declared but never defined

private:
    Ui::statistics* ui;
    BudgetManager*  manager    = nullptr;
    Addtransaction* mainWindow = nullptr;

    // Helper: update a single category row (spent label + remaining label)
    void updateCategoryRow(const QString& category,
                           const QMap<QString, double>& totals,
                           QLabel* spentLabel,
                           QLineEdit* budgetInput,
                           QLabel* remainingLabel,
                           const QString& currency);
};

#endif
