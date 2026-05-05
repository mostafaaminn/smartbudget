#include "statistics.h"
#include "ui_statistics.h"

#include <QMap>
#include <QDate>
#include <QCloseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>

#include "budgetmanager.h"
#include "transaction.h"
#include "addtransaction.h"

statistics::statistics(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::statistics)
{
    ui->setupUi(this);


    const auto budgetInputs = {
        ui->budgetFood,
        ui->budgetRent,
        ui->budgetTransportation,
        ui->budgetOther,
        ui->budgetHealth,
        ui->budgetGroceries,
        ui->budgetEntertainment,
        ui->budgetEducation,
        ui->budgetUtilities
    };

    for (QLineEdit* input : budgetInputs) {
        connect(input, &QLineEdit::textChanged,
                this, &statistics::updateStats);
    }
}

statistics::~statistics()
{
    delete ui;
}

void statistics::setManager(BudgetManager* m)
{
    manager = m;

    if (manager) {
        auto fill = [&](QLineEdit* input, const QString& cat) {
            double saved = manager->getBudget(cat);
            if (saved > 0) {
                input->blockSignals(true);
                input->setText(QString::number(saved, 'f', 2));
                input->blockSignals(false);
            }
        };

        fill(ui->budgetFood,          "Food");
        fill(ui->budgetRent,          "Rent");
        fill(ui->budgetTransportation,"Transportation");
        fill(ui->budgetOther,         "Other");
        fill(ui->budgetHealth,        "Health");
        fill(ui->budgetGroceries,     "Groceries");
        fill(ui->budgetEntertainment, "Entertainment");
        fill(ui->budgetEducation,     "Education");
        fill(ui->budgetUtilities,     "Utilities");
    }

    updateStats();
}

void statistics::setMainWindow(Addtransaction* w)
{
    mainWindow = w;
}

void statistics::updateCategoryRow(const QString& category,
                                   const QMap<QString, double>& totals,
                                   QLabel*    spentLabel,
                                   QLineEdit* budgetInput,
                                   QLabel*    remainingLabel,
                                   const QString& currency)
{
    double spent = totals.value(category, 0.0);
    spentLabel->setText(QString::number(spent, 'f', 2) + " " + currency);

    bool ok = false;
    double budget = budgetInput->text().toDouble(&ok);

    if (manager) {
        if (ok && budget > 0)
            manager->setBudget(category, budget);
        else if (budgetInput->text().trimmed().isEmpty())
            manager->clearBudget(category);
    }

    double remaining = manager ? manager->budgetDifference(category) : 0.0;
    remainingLabel->setText(QString::number(remaining, 'f', 2) + " " + currency);
    remainingLabel->setStyleSheet(remaining < 0 ? "color: red;" : "color: green;");
}

void statistics::updateStats()
{
    if (!manager) return;

    QDate   now      = QDate::currentDate();
    QString currency = manager->getDisplayCurrency();

    QMap<QString, double> categoryExpenses;
    double monthIncome   = 0.0;
    double monthExpenses = 0.0;
    int    totalCount    = 0;

    for (const Transaction& t : manager->getAllTransactions()) {
        QString type     = t.getType().trimmed().toLower();
        QString category = t.getCategory().trimmed();

        if (category.compare("Transport", Qt::CaseInsensitive) == 0)
            category = "Transportation";

        double amount = manager->convertToDisplay(t.getAmount(), t.getCurrency());
        ++totalCount;

        if (t.getDate().month() == now.month() &&
            t.getDate().year()  == now.year()) {
            if (type == "income")       monthIncome   += amount;
            else if (type == "expense") monthExpenses += amount;
        }

        if (type == "expense") categoryExpenses[category] += amount;
    }

    ui->monthIncome->setText(  QString::number(monthIncome,               'f', 2) + " " + currency);
    ui->monthExpenses->setText(QString::number(monthExpenses,             'f', 2) + " " + currency);
    ui->monthSaving->setText(  QString::number(monthIncome - monthExpenses,'f', 2) + " " + currency);
    ui->totalTransactions->setText(QString::number(totalCount));

    QString highestCategory = "None";
    double  highestValue    = 0.0;
    for (auto it = categoryExpenses.constBegin(); it != categoryExpenses.constEnd(); ++it) {
        if (it.value() > highestValue) { highestValue = it.value(); highestCategory = it.key(); }
    }
    ui->Highestspending->setText(highestCategory);

    updateCategoryRow("Utilities",      categoryExpenses, ui->Utilities,      ui->budgetUtilities,      ui->remainingUtilities,      currency);
    updateCategoryRow("Groceries",      categoryExpenses, ui->Groceries,      ui->budgetGroceries,      ui->remainingGroceries,      currency);
    updateCategoryRow("Rent",           categoryExpenses, ui->Rent,           ui->budgetRent,           ui->remainingRent,           currency);
    updateCategoryRow("Food",           categoryExpenses, ui->Food,           ui->budgetFood,           ui->remainingFood,           currency);
    updateCategoryRow("Entertainment",  categoryExpenses, ui->Entertainment,  ui->budgetEntertainment,  ui->remainingEntertainment,  currency);
    updateCategoryRow("Education",      categoryExpenses, ui->Education,      ui->budgetEducation,      ui->remainingEducation,      currency);
    updateCategoryRow("Health",         categoryExpenses, ui->Health,         ui->budgetHealth,         ui->remainingHealth,         currency);
    updateCategoryRow("Transportation", categoryExpenses, ui->Transportation, ui->budgetTransportation, ui->remainingTransportation, currency);
    updateCategoryRow("Other",          categoryExpenses, ui->Other,          ui->budgetOther,          ui->remainingOther,          currency);
}

void statistics::showMonthComparison()
{
    if (!manager) return;

    bool ok1 = false, ok2 = false;

    QString m1Str = QInputDialog::getText(this, "Compare Months",
                                          "Enter first month (yyyy-MM):", QLineEdit::Normal, "", &ok1);
    if (!ok1 || m1Str.isEmpty()) return;

    QString m2Str = QInputDialog::getText(this, "Compare Months",
                                          "Enter second month (yyyy-MM):", QLineEdit::Normal, "", &ok2);
    if (!ok2 || m2Str.isEmpty()) return;

    QDate m1 = QDate::fromString(m1Str + "-01", "yyyy-MM-dd");
    QDate m2 = QDate::fromString(m2Str + "-01", "yyyy-MM-dd");

    if (!m1.isValid() || !m2.isValid()) {
        QMessageBox::warning(this, "Invalid Input",
                             "Please enter dates in yyyy-MM format (e.g. 2026-03).");
        return;
    }

    QString summary = manager->compareMonthSummary(m1, m2);
    QMessageBox::information(this, "Month Comparison", summary);
}

void statistics::showSavingsGoals()
{
    if (!manager) return;

    const auto& goals = manager->getSavingsGoals();
    if (goals.empty()) {
        QMessageBox::information(this, "Savings Goals", "No savings goals set yet.");
    } else {
        QString text;
        for (int i = 0; i < static_cast<int>(goals.size()); ++i) {
            const SavingsGoal& g = goals[i];
            double pct = g.targetAmount > 0
                             ? (g.savedAmount / g.targetAmount * 100.0) : 0.0;
            text += QString("%1. %2: %3 / %4 (%5%)\n")
                        .arg(i + 1).arg(g.name)
                        .arg(QString::number(g.savedAmount, 'f', 2))
                        .arg(QString::number(g.targetAmount, 'f', 2))
                        .arg(QString::number(pct, 'f', 1));
        }
        QMessageBox::information(this, "Savings Goals", text);
    }
}

void statistics::addSavingsGoal()
{
    if (!manager) return;

    bool ok = false;
    QString name = QInputDialog::getText(this, "Add Savings Goal",
                                         "Goal name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString targetStr = QInputDialog::getText(this, "Add Savings Goal",
                                              "Target amount:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    double target = targetStr.toDouble(&ok);
    if (!ok || target <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Enter a valid positive target amount.");
        return;
    }

    manager->addSavingsGoal({name, target, 0.0});
    QMessageBox::information(this, "Savings Goal Added",
                             QString("Goal \"%1\" added with target %2.").arg(name).arg(target));
}

void statistics::showBillReminders()
{
    if (!manager) return;

    QStringList upcoming = manager->getUpcomingBills(QDate::currentDate(), 30);
    if (upcoming.isEmpty()) {
        QMessageBox::information(this, "Bill Reminders",
                                 "No bills due in the next 30 days.");
    } else {
        QMessageBox::information(this, "Upcoming Bills (next 30 days)",
                                 upcoming.join("\n"));
    }
}

void statistics::addBillReminder()
{
    if (!manager) return;

    bool ok = false;
    QString name = QInputDialog::getText(this, "Add Bill Reminder",
                                         "Bill name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString amountStr = QInputDialog::getText(this, "Add Bill Reminder",
                                              "Amount:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    double amount = amountStr.toDouble(&ok);
    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Enter a valid positive amount.");
        return;
    }

    int day = QInputDialog::getInt(this, "Add Bill Reminder",
                                   "Day of month (1-31):", 1, 1, 31, 1, &ok);
    if (!ok) return;

    QStringList currencies = {"USD", "EGP"};
    QString currency = QInputDialog::getItem(this, "Add Bill Reminder",
                                             "Currency:", currencies, 0, false, &ok);
    if (!ok) return;

    manager->addBillReminder({name, amount, day, currency});
    QMessageBox::information(this, "Bill Reminder Added",
                             QString("Reminder for \"%1\" added on day %2 of each month.").arg(name).arg(day));
}


void statistics::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    if (mainWindow) { mainWindow->refreshLabels(); mainWindow->show(); }
    QWidget::closeEvent(event);
}
