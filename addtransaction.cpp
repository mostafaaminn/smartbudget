#include "addtransaction.h"
#include "ui_addtransaction.h"

#include <QMessageBox>
#include <QDate>
#include <QStringList>

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
                QMessageBox::warning(this, "Network Error", error);
            });

    connect(networkClient, &NetworkClient::messageSent,
            this, [this]() {
                QMessageBox::information(this, "Network", "Transaction sent to server successfully.");
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

    QString jsonMessage = buildTransactionJson(amount, type, category, date);
    networkClient->sendMessage((jsonMessage + "\n").toStdString());

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

QString Addtransaction::buildTransactionJson(double amount, const QString& type, const QString& category, const QDate& date) const
{
    QString safeType = type;
    QString safeCategory = category;

    safeType.replace("\\", "\\\\");
    safeType.replace("\"", "\\\"");

    safeCategory.replace("\\", "\\\\");
    safeCategory.replace("\"", "\\\"");

    return QString("{\"action\":\"add_transaction\",\"type\":\"%1\",\"category\":\"%2\",\"amount\":%3,\"date\":\"%4\"}")
        .arg(safeType)
        .arg(safeCategory)
        .arg(QString::number(amount, 'f', 2))
        .arg(date.toString(Qt::ISODate));
}

BudgetManager* Addtransaction::getManager()
{
    return &manager;
}
