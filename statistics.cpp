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
        QString category = t.getCategory().trimmed();

        if (category.compare("Transport", Qt::CaseInsensitive) == 0) {
            category = "Transportation";
        }

        if (t.getDate().month() == now.month() && t.getDate().year() == now.year())
        {
            if (type == "income") {
                monthIncome += amount;
            } else if (type == "expense") {
                monthExpenses += amount;
            }
        }

        if (type == "expense")
        {
            categoryTotals[category] += amount;
        }
    }

    int totalTransactions = static_cast<int>(manager->getAllTransactions().size());
    double monthBalance = monthIncome - monthExpenses;

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
    ui->Transportation->setText(QString::number(categoryTotals["Transportation"]));
    ui->Utilities->setText(QString::number(categoryTotals["Utilities"]));

    double foodBudget = ui->budgetFood->text().toDouble();
    manager->setBudget("Food", foodBudget);
    ui->remainingFood->setText(QString::number(foodBudget - categoryTotals["Food"]));

    double rentBudget = ui->budgetRent->text().toDouble();
    manager->setBudget("Rent", rentBudget);
    ui->remainingRent->setText(QString::number(rentBudget - categoryTotals["Rent"]));

    double groceriesBudget = ui->budgetGroceries->text().toDouble();
    manager->setBudget("Groceries", groceriesBudget);
    ui->remainingGroceries->setText(QString::number(groceriesBudget - categoryTotals["Groceries"]));

    double transportationBudget = ui->budgetTransportation->text().toDouble();
    manager->setBudget("Transportation", transportationBudget);
    ui->remainingTransportation->setText(QString::number(transportationBudget - categoryTotals["Transportation"]));

    double utilitiesBudget = ui->budgetUtilities->text().toDouble();
    manager->setBudget("Utilities", utilitiesBudget);
    ui->remainingUtilities->setText(QString::number(utilitiesBudget - categoryTotals["Utilities"]));

    double otherBudget = ui->budgetOther->text().toDouble();
    manager->setBudget("Other", otherBudget);
    ui->remainingOther->setText(QString::number(otherBudget - categoryTotals["Other"]));

    double entertainmentBudget = ui->budgetEntertainment->text().toDouble();
    manager->setBudget("Entertainment", entertainmentBudget);
    ui->remainingEntertainment->setText(QString::number(entertainmentBudget - categoryTotals["Entertainment"]));

    double healthBudget = ui->budgetHealth->text().toDouble();
    manager->setBudget("Health", healthBudget);
    ui->remainingHealth->setText(QString::number(healthBudget - categoryTotals["Health"]));

    double educationBudget = ui->budgetEducation->text().toDouble();
    manager->setBudget("Education", educationBudget);
    ui->remainingEducation->setText(QString::number(educationBudget - categoryTotals["Education"]));
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
