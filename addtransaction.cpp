#include "addtransaction.h"
#include "ui_addtransaction.h"

#include <QMessageBox>
#include <QDate>
#include <QStringList>

#include "JsonTools.h"
#include "transaction.h"
#include "history.h"
#include "statistics.h"

Addtransaction::Addtransaction(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Addtransaction)
    , networkClient(new NetworkClient(this))
{
    ui->setupUi(this);

    setWindowTitle("SmartBudget - Add Transaction");

    connect(ui->pushButton, &QPushButton::clicked,
            this, &Addtransaction::onAddClicked);

    connect(ui->historyButton, &QPushButton::clicked,
            this, &Addtransaction::on_historyButton_clicked);

    connect(ui->Summary, &QPushButton::clicked,
            this, &Addtransaction::on_summaryButton_clicked);

    connect(ui->comboDisplayCurrency, &QComboBox::currentTextChanged,
            this, &Addtransaction::onDisplayCurrencyChanged);

    connect(networkClient, &NetworkClient::statusChanged,
            ui->networkStatusLabel, &QLabel::setText);

    connect(networkClient, &NetworkClient::errorOccurred,
            this, [this](const QString& error) {
                Q_UNUSED(error);
            });

    connect(networkClient, &NetworkClient::messageSent,
            this, [this]() {
            });

    networkClient->connectToServer("127.0.0.1", 12345);

    refreshLabels();
}

Addtransaction::~Addtransaction()
{
    delete ui;
}

void Addtransaction::onAddClicked()
{
    QString amountText = ui->amountInput->text().trimmed();
    QString type = ui->comboBox->currentText().trimmed().toLower();
    QString category = ui->categoryBox->currentText().trimmed();
    QString currency = ui->comboCurrency->currentText().trimmed();

    if (amountText.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter an amount.");
        return;
    }

    bool ok = false;
    double amount = amountText.toDouble(&ok);

    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid positive number for the amount.");
        return;
    }

    if (type != "income" && type != "expense") {
        QMessageBox::warning(this, "Invalid Input", "Please choose either income or expense.");
        return;
    }

    if (category == "Category" || category.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please choose a category.");
        return;
    }

    if (currency == "Currency" || currency.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please choose a currency.");
        return;
    }

    QDate date = QDate::currentDate();
    Transaction newTransaction(amount, type, category, date, currency);

    manager.addTransaction(newTransaction);

    refreshLabels();

    emit totalsChanged(
        manager.getBalance(),
        manager.getTotalIncome(),
        manager.getTotalExpenses(),
        manager.getTransactionCount(),
        manager.getHighestSpendingCategory()
        );

    networkClient->sendMessage(JsonTools::addTransactionMsg(newTransaction));

    QMessageBox::information(this, "Success", "Transaction added successfully.");

    clearInputs();
}

void Addtransaction::clearInputs()
{
    ui->amountInput->clear();
    ui->comboBox->setCurrentIndex(0);
    ui->categoryBox->setCurrentIndex(0);
    ui->comboCurrency->setCurrentIndex(0);
}

void Addtransaction::updateSummaryLabels()
{
    refreshLabels();
}

void Addtransaction::refreshLabels()
{
    QString cur = manager.getDisplayCurrency();

    ui->balanceValueLabel->setText(QString::number(manager.getBalance(), 'f', 2) + " " + cur);
    ui->incomeValueLabel->setText(QString::number(manager.getTotalIncome(), 'f', 2) + " " + cur);
    ui->expensesValueLabel->setText(QString::number(manager.getTotalExpenses(), 'f', 2) + " " + cur);
}

BudgetManager* Addtransaction::getManager()
{
    return &manager;
}

void Addtransaction::on_historyButton_clicked()
{
    if (!historyWindow) {
        historyWindow = new History();
        historyWindow->setManager(&manager);
        historyWindow->setMainWindow(this);
    }

    historyWindow->refreshTable();
    historyWindow->show();
    this->hide();
}

void Addtransaction::on_summaryButton_clicked()
{
    if (!statsWindow) {
        statsWindow = new statistics();
        statsWindow->setManager(&manager);
        statsWindow->setMainWindow(this);
    }

    statsWindow->updateStats();
    statsWindow->show();
    this->hide();
}

void Addtransaction::onDisplayCurrencyChanged(const QString& text)
{
    QString cur = text;
    cur.remove("Display-");
    cur = cur.trimmed().toUpper();

    if (cur == "CURRENCY") {
        return;
    }

    manager.setDisplayCurrency(cur);
    refreshLabels();

    if (historyWindow && historyWindow->isVisible()) {
        historyWindow->refreshTable();
    }

    if (statsWindow && statsWindow->isVisible()) {
        statsWindow->updateStats();
    }
}
