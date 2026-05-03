#include "history.h"
#include "ui_history.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>

#include "addtransaction.h"
#include "transaction.h"

History::History(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::History)
{
    ui->setupUi(this);

    connect(ui->deleteButton, &QPushButton::clicked,
            this, &History::on_deleteButton_clicked);

    connect(ui->backButton, &QPushButton::clicked,
            this, &History::on_backButton_clicked);

    connect(ui->editButton, &QPushButton::clicked,
            this, &History::on_editButton_clicked);

    connect(ui->filterBox, &QComboBox::currentTextChanged,
            this, &History::refreshTable);
}

History::~History()
{
    delete ui;
}

void History::setManager(BudgetManager* m)
{
    manager = m;
}

void History::setMainWindow(Addtransaction* w)
{
    mainWindow = w;
}

void History::setTransactions(const std::vector<Transaction>& transactions)
{
    ui->historyTable->setRowCount(0);

    for (const Transaction& t : transactions) {
        int row = ui->historyTable->rowCount();
        ui->historyTable->insertRow(row);

        ui->historyTable->setItem(row, 0, new QTableWidgetItem(QString::number(t.getAmount())));
        ui->historyTable->setItem(row, 1, new QTableWidgetItem(t.getCategory()));
        ui->historyTable->setItem(row, 2, new QTableWidgetItem(t.getType()));
        ui->historyTable->setItem(row, 3, new QTableWidgetItem(t.getDate().toString("yyyy-MM-dd")));
        ui->historyTable->setItem(row, 4, new QTableWidgetItem(t.getCurrency()));
    }
}

void History::refreshTable()
{
    ui->historyTable->setRowCount(0);

    if (!manager) {
        return;
    }

    QString filter = ui->filterBox->currentText().trimmed().toLower();
    QString displayCur = manager->getDisplayCurrency();
    auto transactions = manager->getAllTransactions();

    for (const Transaction& t : transactions) {
        QString type = t.getType().trimmed().toLower();

        if (filter != "all" && type != filter) {
            continue;
        }

        int row = ui->historyTable->rowCount();
        ui->historyTable->insertRow(row);

        double displayAmt = manager->convertToDisplay(t.getAmount(), t.getCurrency());
        QString amountStr = QString::number(displayAmt, 'f', 2) + " " + displayCur;

        ui->historyTable->setItem(row, 0, new QTableWidgetItem(amountStr));
        ui->historyTable->setItem(row, 1, new QTableWidgetItem(t.getCategory()));
        ui->historyTable->setItem(row, 2, new QTableWidgetItem(t.getType()));
        ui->historyTable->setItem(row, 3, new QTableWidgetItem(t.getDate().toString("yyyy-MM-dd")));
        ui->historyTable->setItem(row, 4, new QTableWidgetItem(t.getCurrency()));
    }
}

void History::on_backButton_clicked()
{
    this->hide();

    if (mainWindow) {
        mainWindow->refreshLabels();
        mainWindow->show();
    }
}

void History::on_deleteButton_clicked()
{
    int row = ui->historyTable->currentRow();

    if (row < 0) {
        QMessageBox::information(this, "Delete", "Please select a row to delete.");
        return;
    }

    if (!manager) {
        return;
    }

    QString filter = ui->filterBox->currentText().trimmed().toLower();
    auto transactions = manager->getAllTransactions();

    int managerIndex = -1;
    int visibleCount = 0;

    for (int i = 0; i < static_cast<int>(transactions.size()); ++i) {
        QString type = transactions[i].getType().trimmed().toLower();

        if (filter != "all" && type != filter) {
            continue;
        }

        if (visibleCount == row) {
            managerIndex = i;
            break;
        }

        ++visibleCount;
    }

    if (managerIndex < 0) {
        return;
    }

    manager->removeTransaction(managerIndex);
    refreshTable();

    if (mainWindow) {
        mainWindow->refreshLabels();
    }
}

void History::on_editButton_clicked()
{
    int row = ui->historyTable->currentRow();

    if (row < 0) {
        QMessageBox::information(this, "Edit", "Please select a row to edit.");
        return;
    }

    if (!manager) {
        return;
    }

    QString filter = ui->filterBox->currentText().trimmed().toLower();
    auto transactions = manager->getAllTransactions();

    int managerIndex = -1;
    int visibleCount = 0;

    for (int i = 0; i < static_cast<int>(transactions.size()); ++i) {
        QString type = transactions[i].getType().trimmed().toLower();

        if (filter != "all" && type != filter) {
            continue;
        }

        if (visibleCount == row) {
            managerIndex = i;
            break;
        }

        ++visibleCount;
    }

    if (managerIndex < 0) {
        return;
    }

    bool okAmount = false;
    QString amountStr = QInputDialog::getText(
        this,
        "Edit Transaction",
        "New Amount:",
        QLineEdit::Normal,
        QString::number(transactions[managerIndex].getAmount()),
        &okAmount
        );

    if (!okAmount) {
        return;
    }

    bool amountValid = false;
    double amount = amountStr.trimmed().toDouble(&amountValid);

    if (!amountValid || amount <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid positive amount.");
        return;
    }

    bool okType = false;
    QString type = QInputDialog::getText(
                       this,
                       "Edit Transaction",
                       "New Type (income/expense):",
                       QLineEdit::Normal,
                       transactions[managerIndex].getType(),
                       &okType
                       ).trimmed().toLower();

    if (!okType) {
        return;
    }

    if (type != "income" && type != "expense") {
        QMessageBox::warning(this, "Invalid Input", "Type must be either income or expense.");
        return;
    }

    bool okCategory = false;
    QString category = QInputDialog::getText(
                           this,
                           "Edit Transaction",
                           "New Category:",
                           QLineEdit::Normal,
                           transactions[managerIndex].getCategory(),
                           &okCategory
                           ).trimmed();

    if (!okCategory) {
        return;
    }

    if (category.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Category cannot be empty.");
        return;
    }

    QString currency = transactions[managerIndex].getCurrency();

    manager->updateTransaction(managerIndex, amount, type, category, currency);
    refreshTable();

    if (mainWindow) {
        mainWindow->refreshLabels();
    }
}

void History::closeEvent(QCloseEvent *event)
{
    emit windowClosed();

    if (mainWindow) {
        mainWindow->refreshLabels();
        mainWindow->show();
    }

    QWidget::closeEvent(event);
}
