#include "statistics.h"
#include "ui_statistics.h"

#include <QMap>
#include <QDate>
#include <QCloseEvent>
#include <QLabel>
#include <QLineEdit>

#include "budgetmanager.h"
#include "transaction.h"
#include "addtransaction.h"

statistics::statistics(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::statistics)
{
    ui->setupUi(this);

    connect(ui->back1Button, &QPushButton::clicked,
            this, &statistics::on_back1Button_clicked);

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
    updateStats();
}

void statistics::setMainWindow(Addtransaction* w)
{
    mainWindow = w;
}

void statistics::updateCategoryRow(const QString& category,
                                   const QMap<QString, double>& totals,
                                   QLabel* spentLabel,
                                   QLineEdit* budgetInput,
                                   QLabel* remainingLabel,
                                   const QString& currency)
{
    double spent = totals.value(category, 0.0);
    spentLabel->setText(QString::number(spent, 'f', 2) + " " + currency);

    bool ok = false;
    double budget = budgetInput->text().toDouble(&ok);

    if (ok && budget > 0 && manager) {
        manager->setBudget(category, budget);
    }

    double remaining = manager ? manager->budgetDifference(category) : 0.0;
    remainingLabel->setText(QString::number(remaining, 'f', 2) + " " + currency);

    if (remaining < 0) {
        remainingLabel->setStyleSheet("color: red;");
    } else {
        remainingLabel->setStyleSheet("color: green;");
    }
}

void statistics::updateStats()
{
    if (!manager) {
        return;
    }

    QDate now = QDate::currentDate();
    QString currency = manager->getDisplayCurrency();

    QMap<QString, double> categoryExpenses;
    double monthIncome = 0.0;
    double monthExpenses = 0.0;
    int totalCount = 0;

    for (const Transaction& t : manager->getAllTransactions()) {
        QString type = t.getType().trimmed().toLower();
        QString category = t.getCategory().trimmed();

        if (category.compare("Transport", Qt::CaseInsensitive) == 0) {
            category = "Transportation";
        }

        double amount = manager->convertToDisplay(t.getAmount(), t.getCurrency());
        ++totalCount;

        if (t.getDate().month() == now.month() &&
            t.getDate().year() == now.year()) {
            if (type == "income") {
                monthIncome += amount;
            } else if (type == "expense") {
                monthExpenses += amount;
            }
        }

        if (type == "expense") {
            categoryExpenses[category] += amount;
        }
    }

    ui->monthIncome->setText(QString::number(monthIncome, 'f', 2) + " " + currency);
    ui->monthExpenses->setText(QString::number(monthExpenses, 'f', 2) + " " + currency);
    ui->monthSaving->setText(QString::number(monthIncome - monthExpenses, 'f', 2) + " " + currency);

    ui->totalTransactions->setText(QString::number(totalCount));

    QString highestCategory = "None";
    double highestValue = 0.0;

    for (auto it = categoryExpenses.constBegin(); it != categoryExpenses.constEnd(); ++it) {
        if (it.value() > highestValue) {
            highestValue = it.value();
            highestCategory = it.key();
        }
    }

    ui->Highestspending->setText(highestCategory);

    updateCategoryRow("Utilities", categoryExpenses,
                      ui->Utilities, ui->budgetUtilities, ui->remainingUtilities, currency);

    updateCategoryRow("Groceries", categoryExpenses,
                      ui->Groceries, ui->budgetGroceries, ui->remainingGroceries, currency);

    updateCategoryRow("Rent", categoryExpenses,
                      ui->Rent, ui->budgetRent, ui->remainingRent, currency);

    updateCategoryRow("Food", categoryExpenses,
                      ui->Food, ui->budgetFood, ui->remainingFood, currency);

    updateCategoryRow("Entertainment", categoryExpenses,
                      ui->Entertainment, ui->budgetEntertainment, ui->remainingEntertainment, currency);

    updateCategoryRow("Education", categoryExpenses,
                      ui->Education, ui->budgetEducation, ui->remainingEducation, currency);

    updateCategoryRow("Health", categoryExpenses,
                      ui->Health, ui->budgetHealth, ui->remainingHealth, currency);

    updateCategoryRow("Transportation", categoryExpenses,
                      ui->Transportation, ui->budgetTransportation, ui->remainingTransportation, currency);

    updateCategoryRow("Other", categoryExpenses,
                      ui->Other, ui->budgetOther, ui->remainingOther, currency);
}

void statistics::on_back1Button_clicked()
{
    this->hide();

    if (mainWindow) {
        mainWindow->refreshLabels();
        mainWindow->show();
    }
}

void statistics::closeEvent(QCloseEvent *event)
{
    emit windowClosed();

    if (mainWindow) {
        mainWindow->refreshLabels();
        mainWindow->show();
    }

    QWidget::closeEvent(event);
}
