#include "statistics.h"
#include "ui_statistics.h"
#include "budgetmanager.h"
#include "transaction.h"
#include "addtransaction.h"
#include <QDate>
#include <QMap>
#include <QLabel>
#include <QLineEdit>

statistics::statistics(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::statistics)
{
    ui->setupUi(this);

    connect(ui->back1Button, &QPushButton::clicked,
            this, &statistics::on_back1Button_clicked);

    // FIX: each budget input change triggers a full stats refresh
    const auto budgetInputs = {
        ui->budgetUtilities,    ui->budgetGroceries,
        ui->budgetRent,         ui->budgetFood,
        ui->budgetEntertainment,ui->budgetEducation,
        ui->budgetHealth,       ui->budgetTransportation,
        ui->budgetOther
    };
    for (QLineEdit* input : budgetInputs)
        connect(input, &QLineEdit::textChanged, this, &statistics::updateStats);
}

statistics::~statistics()
{
    delete ui;
}

void statistics::setManager(BudgetManager* m)
{
    manager = m;
    updateStats();
}

void statistics::setMainWindow(Addtransaction* w)
{
    mainWindow = w;
}

// ---------------------------------------------------------------------------
// Helper — update one category row
// ---------------------------------------------------------------------------
void statistics::updateCategoryRow(const QString& category,
                                   const QMap<QString, double>& totals,
                                   QLabel*    spentLabel,
                                   QLineEdit* budgetInput,
                                   QLabel*    remainingLabel,
                                   const QString& currency)
{
    double spent = totals.value(category, 0.0);
    spentLabel->setText(QString::number(spent, 'f', 2) + " " + currency);

    // Read the budget the user typed; push it into the manager
    bool   ok     = false;
    double budget = budgetInput->text().toDouble(&ok);
    if (ok && budget > 0)
        manager->setBudget(category, budget);

    double remaining = manager->budgetDifference(category);
    remainingLabel->setText(QString::number(remaining, 'f', 2) + " " + currency);

    // FIX: colour the remaining label red when over-budget
    if (remaining < 0)
        remainingLabel->setStyleSheet("color: red;");
    else
        remainingLabel->setStyleSheet("color: green;");
}

// ---------------------------------------------------------------------------
// Main stats refresh
// ---------------------------------------------------------------------------
void statistics::updateStats()
{
    if (!manager) return;

    QDate now = QDate::currentDate();
    QString currency = manager->getDisplayCurrency();

    // FIX: separate maps for category totals (expenses only) vs all-time totals
    QMap<QString, double> categoryExpenses;   // expenses only, all time
    double monthIncome   = 0.0;
    double monthExpenses = 0.0;
    int    totalCount    = 0;

    for (const Transaction& t : manager->getAllTransactions())
    {
        QString type  = t.getType().trimmed().toLower();
        double  amount = manager->convertToDisplay(t.getAmount(), t.getCurrency());
        ++totalCount;

        // Monthly summary
        if (t.getDate().month() == now.month() &&
            t.getDate().year()  == now.year())
        {
            if (type == "income")  monthIncome   += amount;
            if (type == "expense") monthExpenses += amount;
        }

        // FIX: only count EXPENSES for the per-category spending totals
        if (type == "expense")
            categoryExpenses[t.getCategory()] += amount;
    }

    // ---- Monthly summary ----
    ui->monthIncome  ->setText(QString::number(monthIncome,              'f', 2) + " " + currency);
    ui->monthExpenses->setText(QString::number(monthExpenses,            'f', 2) + " " + currency);
    ui->monthSaving  ->setText(QString::number(monthIncome - monthExpenses, 'f', 2) + " " + currency);

    // FIX: update total transactions count
    ui->totalTransactions->setText(QString::number(totalCount));

    // FIX: find and display highest spending category
    QString highestCat;
    double  highestAmt = -1.0;
    for (auto it = categoryExpenses.constBegin(); it != categoryExpenses.constEnd(); ++it)
    {
        if (it.value() > highestAmt)
        {
            highestAmt = it.value();
            highestCat = it.key();
        }
    }
    ui->Highestspending->setText(highestCat.isEmpty() ? "N/A" : highestCat);

    // ---- Per-category rows (spent + remaining budget) ----
    // FIX: all rows now properly show converted amounts and remaining budget
    updateCategoryRow("Utilities",     categoryExpenses,
                      ui->Utilities,     ui->budgetUtilities,     ui->remainingUtilities,     currency);
    updateCategoryRow("Groceries",     categoryExpenses,
                      ui->Groceries,     ui->budgetGroceries,     ui->remainingGroceries,     currency);
    updateCategoryRow("Rent",          categoryExpenses,
                      ui->Rent,          ui->budgetRent,          ui->remainingRent,          currency);
    updateCategoryRow("Food",          categoryExpenses,
                      ui->Food,          ui->budgetFood,          ui->remainingFood,          currency);
    updateCategoryRow("Entertainment", categoryExpenses,
                      ui->Entertainment, ui->budgetEntertainment, ui->remainingEntertainment, currency);
    updateCategoryRow("Education",     categoryExpenses,
                      ui->Education,     ui->budgetEducation,     ui->remainingEducation,     currency);
    updateCategoryRow("Health",        categoryExpenses,
                      ui->Health,        ui->budgetHealth,        ui->remainingHealth,        currency);
    updateCategoryRow("Transportation",categoryExpenses,
                      ui->Transportation,ui->budgetTransportation,ui->remainingTransportation,currency);
    updateCategoryRow("Other",         categoryExpenses,
                      ui->Other,         ui->budgetOther,         ui->remainingOther,         currency);
}

void statistics::on_back1Button_clicked()
{
    this->close();
    if (mainWindow)
    {
        mainWindow->refreshLabels();
        mainWindow->show();
    }
}
