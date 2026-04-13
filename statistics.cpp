#include "statistics.h"
#include "ui_statistics.h"

#include <QMap>
#include <QDate>
#include <QCloseEvent>

#include "budgetmanager.h"
#include "transaction.h"
#include "addtransaction.h"

statistics::statistics(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::statistics)
{
    ui->setupUi(this);

    connect(ui->budgetFood, &QLineEdit::textChanged, this, &statistics::updateStats);
    connect(ui->budgetRent, &QLineEdit::textChanged, this, &statistics::updateStats);
    connect(ui->budgetTransportation, &QLineEdit::textChanged, this, &statistics::updateStats);
    connect(ui->budgetOther, &QLineEdit::textChanged, this, &statistics::updateStats);
    connect(ui->budgetHealth, &QLineEdit::textChanged, this, &statistics::updateStats);
    connect(ui->budgetGroceries, &QLineEdit::textChanged, this, &statistics::updateStats);
    connect(ui->budgetEntertainment, &QLineEdit::textChanged, this, &statistics::updateStats);
    connect(ui->budgetEducation, &QLineEdit::textChanged, this, &statistics::updateStats);
    connect(ui->budgetUtilities, &QLineEdit::textChanged, this, &statistics::updateStats);
}

void statistics::setManager(BudgetManager* m)
{
    manager = m;
    updateStats();
}

void statistics::updateStats()
{
    if (!manager) return;

    double monthIncome = 0;
    double monthExpenses = 0;

    QDate now = QDate::currentDate();

    QMap<QString, double> categoryTotals;

    for (const Transaction& t : manager->getAllTransactions())
    {
        QString type = t.getType().trimmed().toLower();
        double amount = t.getAmount();

        // Monthly summary
        if (t.getDate().month() == now.month() &&
            t.getDate().year() == now.year())
        {
            if (type == "income")
                monthIncome += amount;
            else if (type == "expense")
                monthExpenses += amount;
        }

        // Category totals for expenses only
        if (type == "expense")
        {
            categoryTotals[t.getCategory()] += amount;
        }
    }

    int totalTransactions = manager->getAllTransactions().size();
    double monthBalance = monthIncome - monthExpenses;

    // Highest category
    QString highestCategory = "None";
    double highestValue = 0;

    for (auto it = categoryTotals.begin(); it != categoryTotals.end(); ++it)
    {
        if (it.value() > highestValue)
        {
            highestValue = it.value();
            highestCategory = it.key();
        }
    }

    // UI updates
    ui->monthIncome->setText(QString::number(monthIncome));
    ui->monthExpenses->setText(QString::number(monthExpenses));
    ui->monthSaving->setText(QString::number(monthBalance));
    ui->totalTransactions->setText(QString::number(totalTransactions));
    ui->Highestspending->setText(highestCategory);

    ui->Education->setText(QString::number(categoryTotals["Education"]));
    ui->Entertainment->setText(QString::number(categoryTotals["Entertainment"]));
    ui->Food->setText(QString::number(categoryTotals["Food"]));
    ui->Groceries->setText(QString::number(categoryTotals["Groceries"]));
    ui->Health->setText(QString::number(categoryTotals["Health"]));
    ui->Other->setText(QString::number(categoryTotals["Other"]));
    ui->Rent->setText(QString::number(categoryTotals["Rent"]));
    ui->Transportation->setText(QString::number(categoryTotals["Transport"]));
    ui->Utilities->setText(QString::number(categoryTotals["Utilities"]));

    double foodBudget = ui->budgetFood->text().toDouble();
    manager->setBudget("Food", foodBudget);

    double foodSpent = categoryTotals["Food"];
    double foodRemaining = foodBudget - foodSpent;

    ui->remainingFood->setText(QString::number(foodRemaining));

    double rentBudget = ui->budgetRent->text().toDouble();
    manager->setBudget("Rent", rentBudget);

    double rentSpent = categoryTotals["Rent"];
    double rentRemaining = rentBudget - rentSpent;

    ui->remainingRent->setText(QString::number(rentRemaining));

    double groceriesBudget = ui->budgetGroceries->text().toDouble();
    manager->setBudget("Groceries", groceriesBudget);

    double groceriesSpent = categoryTotals["Groceries"];
    double groceriesRemaining = groceriesBudget - groceriesSpent;

    ui->remainingGroceries->setText(QString::number(groceriesRemaining));

    double transportationBudget = ui->budgetTransportation->text().toDouble();
    manager->setBudget("Transportation", transportationBudget);

    double transportationSpent = categoryTotals["Transportation"];
    double transportationRemaining = transportationBudget - transportationSpent;

    ui->remainingTransportation->setText(QString::number(transportationRemaining));

    double utilitiesBudget = ui->budgetUtilities->text().toDouble();
    manager->setBudget("Utilities", utilitiesBudget);

    double utilitiesSpent = categoryTotals["Utilities"];
    double utilitiesRemaining = utilitiesBudget - utilitiesSpent;

    ui->remainingUtilities->setText(QString::number(utilitiesRemaining));

    double otherBudget = ui->budgetOther->text().toDouble();
    manager->setBudget("Other", otherBudget);

    double otherSpent = categoryTotals["Other"];
    double otherRemaining = otherBudget - otherSpent;

    ui->remainingOther->setText(QString::number(otherRemaining));

    double entertainmentBudget = ui->budgetEntertainment->text().toDouble();
    manager->setBudget("Entertainment", entertainmentBudget);

    double entertainmentSpent = categoryTotals["Entertainment"];
    double entertainmentRemaining = entertainmentBudget - entertainmentSpent;

    ui->remainingEntertainment->setText(QString::number(entertainmentRemaining));

    double healthBudget = ui->budgetHealth->text().toDouble();
    manager->setBudget("Health", healthBudget);

    double healthSpent = categoryTotals["Health"];
    double healthRemaining = healthBudget - healthSpent;

    ui->remainingHealth->setText(QString::number(healthRemaining));

    double educationBudget = ui->budgetEducation->text().toDouble();
    manager->setBudget("Education", educationBudget);

    double educationSpent = categoryTotals["Education"];
    double educationRemaining = educationBudget - educationSpent;

    ui->remainingEducation->setText(QString::number(educationRemaining));
}

void statistics::setMainWindow(Addtransaction* w)
{
    mainWindow = w;
}

void statistics::on_back1Button_clicked()
{
    this->hide();
}

statistics::~statistics()
{
    delete ui;
}

void statistics::closeEvent(QCloseEvent *event)
{
    emit windowClosed();

    if (mainWindow) {
        mainWindow->show();
    }

    QWidget::closeEvent(event);
}
