#include "addtransaction.h"
#include "ui_addtransaction.h"

#include <QMessageBox>
#include <QDate>
#include <QStringList>

#include "JsonTools.h"
#include "transaction.h"

Addtransaction::Addtransaction(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Addtransaction)
    , networkClient(new NetworkClient(this))
{
    ui->setupUi(this);

    setWindowTitle("SmartBudget - Add Transaction");

    if (ui->comboBox->count() == 0) {
        ui->comboBox->addItem("income");
        ui->comboBox->addItem("expense");
    }

    connect(ui->pushButton, &QPushButton::clicked,
            this, &Addtransaction::onAddClicked);

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

    updateSummaryLabels();
}

Addtransaction::~Addtransaction()
{
    delete ui;
}

void Addtransaction::onAddClicked()
{
    QString amountText = ui->amountInput->text().trimmed();
    QString type = ui->comboBox->currentText().trimmed().toLower();
    QString category = ui->categoryInput->text().trimmed();

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

    if (category.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a category.");
        return;
    }

    if (type != "income" && type != "expense") {
        QMessageBox::warning(this, "Invalid Input", "Please choose either income or expense.");
        return;
    }

    QDate date = QDate::currentDate();
    Transaction newTransaction(amount, type, category, date);

    manager.addTransaction(newTransaction);
    updateSummaryLabels();

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
    ui->categoryInput->clear();
    ui->comboBox->setCurrentIndex(0);
}

void Addtransaction::updateSummaryLabels()
{
    ui->balanceValueLabel->setText(QString::number(manager.getBalance()));
    ui->incomeValueLabel->setText(QString::number(manager.getTotalIncome()));
    ui->expensesValueLabel->setText(QString::number(manager.getTotalExpenses()));
}

BudgetManager* Addtransaction::getManager()
{
    return &manager;
}
